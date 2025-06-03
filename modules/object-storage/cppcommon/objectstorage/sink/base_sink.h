/**
 * @file base_sink.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-05-30 14:20:36
 */
#pragma once

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <iomanip>
#include <limits>
#include <memory>
#include <mutex>
#include <ostream>
#include <queue>
#include <sstream>
#include <string>
#include <thread>
#include <utility>

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

  static bool IsExists(const std::string &filepath);
  ~SinkFileSystem() {
    Flush();
    Close();
  }
  inline operator bool() { return IsOpen(); }
};

using OnRollFileCallback = std::function<void(std::string)>;

template <typename Record, typename FS = SinkFileSystem<Record>>
class BaseSink {
 public:
  struct Options {
    std::string name;
    std::string path{""};
    bool name_with_date{false};
    bool name_with_hostname{false};
    bool is_rotate{true};
    int max_backup_files{-1};  // -1: unlimited
    int64_t max_rows_per_file{1000000};
    size_t max_inflight_nums{std::numeric_limits<size_t>::max()};
    std::string suffix{"log"};
    OnRollFileCallback on_roll_call_back{};  // callling with last filepath when rolling file
  };

  struct State {
    int file_index{0};
    int64_t current_row_nums{0};
    bool stopped_{false};
  };

  explicit BaseSink(Options &&options)
      : options_(std::move(options)), writer_thread_(&BaseSink::WriteThreadFunc, this) {}

  ~BaseSink() {
    Close();
    ofs_->Close();
    if (options_.on_roll_call_back) {
      options_.on_roll_call_back(rotated_files_.front());
    }
  }

  template <typename T>
  void Write(T &&record) {
    {
      std::unique_lock lock(mutex_);
      cv_.wait(lock, [&] { return state_.stopped_ || queue_.size() < options_.max_inflight_nums; });
      if (state_.stopped_) return;
      queue_.emplace(std::forward<T>(record));
    }
    cv_.notify_one();
  }
  void Close();

 protected:
  void WriteThreadFunc();
  void RollFile();
  std::string NextFilePath();

 protected:
  Options options_;
  State state_{};

  std::mutex mutex_;
  std::condition_variable cv_;
  std::queue<Record> queue_;
  std::thread writer_thread_;
  std::shared_ptr<FS> ofs_;
  std::queue<std::string> rotated_files_{};
};

template <typename Record, typename FS>
void BaseSink<Record, FS>::Close() {
  {
    std::lock_guard lock(mutex_);
    state_.stopped_ = true;
  }
  cv_.notify_all();
  if (writer_thread_.joinable()) writer_thread_.join();
}

template <typename Record, typename FS>
void BaseSink<Record, FS>::WriteThreadFunc() {
  while (true) {
    Record record;
    {
      std::unique_lock lock(mutex_);
      cv_.wait(lock, [&] { return state_.stopped_ || !queue_.empty(); });

      if (queue_.empty() && state_.stopped_) break;
      if (!queue_.empty()) {
        record = std::move(queue_.front());
        queue_.pop();
      } else {
        continue;
      }
    }

    if (!ofs_ || (options_.is_rotate && state_.current_row_nums >= options_.max_rows_per_file)) {
      RollFile();
    }

    if (ofs_) {
      state_.current_row_nums += ofs_->Write(std::forward<Record>(record));
    }
  }
}

template <typename Record, typename FS>
void BaseSink<Record, FS>::RollFile() {
  std::string filepath = NextFilePath();
  ofs_ = std::make_shared<FS>();
  ofs_->Open(filepath);
  if (!ofs_->IsOpen()) {
    throw std::runtime_error("Failed to open file: " + filepath);
  }
  state_.file_index++;
  state_.current_row_nums = 0;
  if (!rotated_files_.empty()) {
    if (options_.on_roll_call_back) {
      options_.on_roll_call_back(rotated_files_.front());
    }
  }
  rotated_files_.push(filepath);
  // checking max backup files
  while (options_.max_backup_files > 0 && static_cast<int>(rotated_files_.size()) > options_.max_backup_files) {
    auto oldest_fp = rotated_files_.front();
    rotated_files_.pop();
    spdlog::info("backup files exceeds the limit . [limit={}, remove={}]", options_.max_backup_files, oldest_fp);
    if (!std::filesystem::remove(oldest_fp)) {
      spdlog::error("remove rotated log file failed. [file={}]", oldest_fp);
    }
  }
}

template <typename Record, typename FS>
std::string BaseSink<Record, FS>::NextFilePath() {
  std::string hostname_str;
  if (options_.name_with_hostname) {
    char hostname[128] = {};
    if (gethostname(hostname, sizeof(hostname)) == 0) {
      hostname_str = hostname;
    }
  }

  std::string date_str;
  if (options_.name_with_date) {
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);
    std::ostringstream date_stream;
    date_stream << std::put_time(&tm, "%Y%m%d_%H%M%S");
    date_str = date_stream.str();
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
    if (!date_str.empty()) {
      filepath << "_" << date_str;
    }
    if (options_.is_rotate) {
      filepath << "_" << state_.file_index;
    }
    filepath << "." << options_.suffix;

    auto dest = filepath.str();
    if (!options_.is_rotate || !FS::IsExists(dest)) {
      return dest;
    }
    ++state_.file_index;
  }
}
}  // namespace cppcommon::os
