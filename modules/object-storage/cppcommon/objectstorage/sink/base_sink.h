/**
 * @file base_sink.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-05-30 14:20:36
 */
#pragma once

#include <fmt/format.h>
#include <unistd.h>

#include <cstdint>
#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <ostream>
#include <queue>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "concurrentqueue/blockingconcurrentqueue.h"
#include "cppcommon/utils/time.h"
#include "spdlog/spdlog.h"

namespace cppcommon::os {

template <typename Record>
class SinkFileSystem {
 public:
  virtual void Open(const std::string &filepath) = 0;
  // @return number of writted lines
  virtual int Write(Record &&record) = 0;
  virtual bool IsOpen() = 0;
  virtual void Close() {}
  virtual void Flush() {}
  static bool IsThreadSafe() { return false; }
  inline static bool IsExists(const std::string &filepath) { return std::filesystem::exists(filepath); }
  virtual ~SinkFileSystem() {
    Flush();
    Close();
  }
  inline operator bool() { return IsOpen(); }
};

enum class RollPeriod {
  UNSPECIFIED,
  SECONDLY = 1000,
  MINITELY = 1000 * 60,
  HOURLY = 1000 * 60 * 60,
  DAILY = 1000 * 60 * 60 * 24,
};

// NOTE: NORMAL: 2025/06/06/02; PARTED: part=2026-06-06/hour=02
enum class TimeRollPathFormat { UNSPECIFIED, NORMAL, PARTED };

inline std::string GenDatePath(int64_t ts_ms, TimeRollPathFormat path_fmt) {
  auto di = cppcommon::DateInfo(ts_ms);
  if (path_fmt == TimeRollPathFormat::PARTED) {
    return di.Format("part=%Y-%m-%d/hour=%H");
  } else {
    return fmt::format("%Y/%m/%d/%H");
  }
}

struct TimeRollPolicy {
  RollPeriod period;
  TimeRollPathFormat path_fmt;

  inline bool IsRoll() {
    if (period == RollPeriod::UNSPECIFIED) return false;
    auto cur = cppcommon::CurrentTsMs();
    if (last_rolling_ts_ms < 0) {
      last_rolling_ts_ms = cur - cur % static_cast<int64_t>(period);
      return false;
    } else if (cur - last_rolling_ts_ms > static_cast<int64_t>(period)) {
      return true;
    } else {
      return false;
    }
  }

  inline void Roll() {
    if (period != RollPeriod::UNSPECIFIED) {
      auto cur = cppcommon::CurrentTsMs();
      if (last_rolling_ts_ms < 0) {
        last_rolling_ts_ms = cur - cur % static_cast<int64_t>(period);
      } else if (cur - last_rolling_ts_ms > static_cast<int64_t>(period)) {
        last_rolling_ts_ms = cur - cur % static_cast<int64_t>(period);
      }
    }
  }

  inline std::string GetDatePath() const { return GenDatePath(last_rolling_ts_ms, path_fmt); }

  inline std::string GetPreviousDatePath() const {
    return GenDatePath(last_rolling_ts_ms - static_cast<int64_t>(period), path_fmt);
  }

  int64_t last_rolling_ts_ms{-1};
};

using OnRollFileCallback = std::function<void(std::string, const TimeRollPolicy &time_roll_policy)>;

inline std::string GetDateFileName() {
  auto di = cppcommon::DateInfo(cppcommon::CurrentTsMs());
  return di.Format("%Y%m%d_%H%M%S");
}

template <typename Record, typename FS = SinkFileSystem<Record>, typename OfsOptions = void>
class BaseSink {
 public:
  struct FileNameOptions {
    bool name_with_date{false};
    bool name_with_hostname{false};
    bool name_with_timestamp{false};
    std::string suffix{"log"};
  };

  struct RollOptions {
    bool is_rotate{true};
    int64_t max_rows_per_file{1000000};
    int max_backup_files{-1};  // -1: unlimited
    TimeRollPolicy time_roll_policy;
  };

  struct Options {
    std::string name;
    std::string path{""};
    FileNameOptions name_options;
    RollOptions roll_options;
    OnRollFileCallback on_roll_callback{};  // callling with last filepath when rolling file
    bool close_in_threads{false};
    int writer_thread_count{1};
    [[no_unique_address]] std::conditional_t<std::is_void_v<OfsOptions>, int, OfsOptions> ofs_options;
  };

  struct State {
    int file_index{0};
    int64_t current_row_nums{0};
    bool stopped_{false};

    inline void Roll() {
      ++file_index;
      current_row_nums = 0;
    }
  };

  explicit BaseSink(Options &&options) : options_(std::move(options)) {
    auto _writer_thread_count = FS::IsThreadSafe() ? options_.writer_thread_count : 1;
    writer_threads_.reserve(_writer_thread_count);
    spdlog::info("base sink {} with writer threads: {}", typeid(FS).name(), _writer_thread_count);
    for (auto i = 0; i < _writer_thread_count; ++i) {
      writer_threads_.emplace_back(&BaseSink::WriteThreadFunc, this);
    }
  }

  virtual ~BaseSink() { Close(); }

  template <typename T>
  void Write(T &&record) {
    if (state_.stopped_) return;
    queue_.enqueue(std::forward<T>(record));
  }

  inline size_t Size() const { return queue_.size_approx(); }

  void Close();

 protected:
  void WriteThreadFunc();
  void RollFile();
  std::string NextFilePath();
  bool IsRoll();
  void RemoveOverflowFiles();
  void CloseCurrentFile();
  void OpenNewFile(const std::string &filepath);

 protected:
  Options options_;
  State state_{};

  std::mutex roll_mtx_;
  std::mutex ofs_mtx_;
  moodycamel::BlockingConcurrentQueue<Record> queue_;
  std::vector<std::thread> writer_threads_;
  std::shared_ptr<FS> ofs_;
  std::queue<std::string> rotated_files_{};

  std::vector<std::thread> close_threads_{};
};

template <typename Record, typename FS, typename OfsOptions>
inline bool BaseSink<Record, FS, OfsOptions>::IsRoll() {
  if (!ofs_) return true;
  if (!options_.roll_options.is_rotate) return false;
  if (state_.current_row_nums >= options_.roll_options.max_rows_per_file) {
    return true;
  }
  if (options_.roll_options.time_roll_policy.IsRoll()) {
    return true;
  }
  return false;
}

template <typename Record, typename FS, typename OfsOptions>
void BaseSink<Record, FS, OfsOptions>::RemoveOverflowFiles() {
  while (options_.roll_options.max_backup_files > 0 &&
         static_cast<int>(rotated_files_.size()) > options_.roll_options.max_backup_files) {
    auto oldest_fp = rotated_files_.front();
    rotated_files_.pop();
    spdlog::info("backup files exceeds the limit . [limit={}, remove={}]", options_.roll_options.max_backup_files,
                 oldest_fp);
    if (!std::filesystem::remove(oldest_fp)) {
      spdlog::error("remove rotated log file failed. [file={}]", oldest_fp);
    }
  }
}

template <typename Record, typename FS, typename OfsOptions>
void BaseSink<Record, FS, OfsOptions>::Close() {
  // write inflight records
  state_.stopped_ = true;
  for (auto &th : writer_threads_) {
    if (th.joinable()) th.join();
  }
  // close current file
  CloseCurrentFile();
  // wait for file closing threads
  for (auto &td : close_threads_) {
    if (td.joinable()) td.join();
  }
  close_threads_.clear();
}

template <typename Record, typename FS, typename OfsOptions>
void BaseSink<Record, FS, OfsOptions>::WriteThreadFunc() {
  while (!state_.stopped_ || queue_.size_approx() != 0) {
    Record item;
    if (queue_.wait_dequeue_timed(item, std::chrono::milliseconds(5))) {
      {
        std::lock_guard lock(roll_mtx_);
        if (IsRoll()) {
          RollFile();
        }
      }
      if (ofs_) {
        if (FS::IsThreadSafe()) {
          state_.current_row_nums += ofs_->Write(std::forward<Record>(item));
        } else {
          std::lock_guard lock(ofs_mtx_);
          state_.current_row_nums += ofs_->Write(std::forward<Record>(item));
        }
      }
    }
  }
}

template <typename Record, typename FS, typename OfsOptions>
void BaseSink<Record, FS, OfsOptions>::OpenNewFile(const std::string &filepath) {
  if constexpr (std::is_void_v<OfsOptions>) {
    ofs_ = std::make_shared<FS>();
  } else {
    ofs_ = std::make_shared<FS>(options_.ofs_options);
  }
  ofs_->Open(filepath);
  if (!ofs_->IsOpen()) {
    throw std::runtime_error("Failed to open file: " + filepath);
  }
}

struct RollMeta {
  bool is_roll{false};
  std::string filepath;
  TimeRollPolicy time_roll_policy;
};

template <typename Record, typename FS, typename OfsOptions>
void BaseSink<Record, FS, OfsOptions>::CloseCurrentFile() {
  RollMeta meta;
  if (!rotated_files_.empty() && options_.on_roll_callback) {
    meta = RollMeta{true, rotated_files_.back(), options_.roll_options.time_roll_policy};
  }

  auto f = [meta = meta, ofs = std::move(ofs_), ops = &options_]() mutable {
    if (ofs) {
      ofs->Close();
      ofs.reset();
    }
    if (meta.is_roll) {
      ops->on_roll_callback(meta.filepath, meta.time_roll_policy);
    }
  };
  if (options_.close_in_threads) {
    close_threads_.emplace_back(std::thread(std::move(f)));
  } else {
    f();
  }
}

template <typename Record, typename FS, typename OfsOptions>
void BaseSink<Record, FS, OfsOptions>::RollFile() {
  std::string filepath = NextFilePath();
  CloseCurrentFile();
  OpenNewFile(filepath);
  state_.Roll();
  options_.roll_options.time_roll_policy.Roll();
  rotated_files_.push(filepath);
  RemoveOverflowFiles();
}

template <typename Record, typename FS, typename OfsOptions>
std::string BaseSink<Record, FS, OfsOptions>::NextFilePath() {
  std::string hostname_str;
  if (options_.name_options.name_with_hostname) {
    char hostname[128] = {};
    if (gethostname(hostname, sizeof(hostname)) == 0) {
      hostname_str = hostname;
    }
  }

  while (true) {
    std::ostringstream filepath;
    if (!options_.path.empty()) {
      filepath << options_.path << "/";
    }
    filepath << options_.name;
    if (!hostname_str.empty()) {
      filepath << "_" << hostname_str;
    }
    if (!options_.name_options.name_with_date) {
      filepath << "_" << GetDateFileName();
    }
    if (options_.name_options.name_with_timestamp) {
      filepath << "_" << cppcommon::CurrentTsMs();
    }
    if (options_.roll_options.is_rotate) {
      filepath << "_" << state_.file_index;
    }
    filepath << "." << options_.name_options.suffix;

    auto dest = filepath.str();
    if (!options_.roll_options.is_rotate || !FS::IsExists(dest)) {
      return dest;
    }
    ++state_.file_index;
  }
}
}  // namespace cppcommon::os
