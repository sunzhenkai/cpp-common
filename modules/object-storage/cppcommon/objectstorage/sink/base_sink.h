/**
 * @file base_sink.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-05-30 14:20:36
 */
#pragma once

#include <fmt/format.h>
#include <unistd.h>

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <ostream>
#include <queue>
#include <shared_mutex>
#include <sstream>
#include <string>
#include <thread>
#include <utility>

#include "cppcommon/utils/time.h"
#include "spdlog/spdlog.h"

namespace cppcommon::os {

template <typename Record>
class SinkFileSystem {
 public:
  virtual void Open(const std::string &filepath) = 0;
  // @return number of writted lines
  virtual int Write(const Record &record) = 0;
  virtual bool IsOpen() = 0;
  virtual void Close() {}
  virtual void Flush() {}

  static bool IsExists(const std::string &filepath);
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

// NORMAL: 2025/06/06/02; PARTED: part=2026-06-06/02
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

template <typename Record, typename FS = SinkFileSystem<Record>>
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
    // size_t max_inflight_nums{std::numeric_limits<size_t>::max()};
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

  explicit BaseSink(Options &&options)
      : options_(std::move(options)), writer_thread_(&BaseSink::WriteThreadFunc, this) {}

  virtual ~BaseSink() {
    Close();
    if (ofs_) {
      ofs_->Close();
      if (state_.current_row_nums > 0 && options_.on_roll_callback && !rotated_files_.empty()) {
        options_.on_roll_callback(rotated_files_.back(), options_.roll_options.time_roll_policy);
      }
    }
  }

  template <typename T>
  void Write(T &&record) {
    if (state_.stopped_) return;
    {
      std::unique_lock lock(queue_mutex_);
      queue_.emplace(std::forward<T>(record));
    }
    cv_.notify_one();
  }

  inline size_t Size() const {
    std::shared_lock lock(queue_mutex_);
    return queue_.size();
  }

  void Close();

 protected:
  void WriteThreadFunc();
  void RollFile();
  std::string NextFilePath();
  bool IsRoll();
  void RemoveOverflowFiles();
  void OpenNewFile(const std::string &filepath);

 protected:
  Options options_;
  State state_{};

  std::condition_variable cv_;
  std::mutex cv_mutex_;
  std::queue<Record> queue_;
  std::shared_mutex queue_mutex_;
  std::thread writer_thread_;
  std::shared_ptr<FS> ofs_;
  std::queue<std::string> rotated_files_{};
};

template <typename Record, typename FS>
inline bool BaseSink<Record, FS>::IsRoll() {
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

template <typename Record, typename FS>
void BaseSink<Record, FS>::RemoveOverflowFiles() {
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

template <typename Record, typename FS>
void BaseSink<Record, FS>::Close() {
  {
    std::lock_guard lock(cv_mutex_);
    state_.stopped_ = true;
  }
  cv_.notify_all();
  if (writer_thread_.joinable()) writer_thread_.join();
}

template <typename Record, typename FS>
void BaseSink<Record, FS>::WriteThreadFunc() {
  while (true) {
    if (queue_.empty() && state_.stopped_) break;
    {
      // try wait only if queue is empty
      if (queue_.empty()) {
        std::unique_lock lock(cv_mutex_);
        cv_.wait(lock, [&] { return state_.stopped_ || !queue_.empty(); });
      } else {
        if (IsRoll()) {
          RollFile();
        }
        if (ofs_) {
          // NOTE: only one write thread (consumer thread)
          state_.current_row_nums += ofs_->Write(queue_.front());
          queue_.front().reset();
        }
        {
          std::unique_lock lock(queue_mutex_);
          queue_.pop();
        }
      }
    }
  }
}

template <typename Record, typename FS>
void BaseSink<Record, FS>::OpenNewFile(const std::string &filepath) {
  if (ofs_) {
    ofs_->Close();
    // std::thread([ofs = std::move(ofs_)]() mutable { ofs->Close(); }).detach();
    ofs_.reset();
  }
  ofs_ = std::make_shared<FS>();
  ofs_->Open(filepath);
  if (!ofs_->IsOpen()) {
    throw std::runtime_error("Failed to open file: " + filepath);
  }
}

template <typename Record, typename FS>
void BaseSink<Record, FS>::RollFile() {
  std::string filepath = NextFilePath();
  OpenNewFile(filepath);
  state_.Roll();
  // trigger on roll callback
  if (!rotated_files_.empty() && options_.on_roll_callback) {
    options_.on_roll_callback(rotated_files_.back(), options_.roll_options.time_roll_policy);
  }
  options_.roll_options.time_roll_policy.Roll();
  rotated_files_.push(filepath);
  RemoveOverflowFiles();
}

template <typename Record, typename FS>
std::string BaseSink<Record, FS>::NextFilePath() {
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
