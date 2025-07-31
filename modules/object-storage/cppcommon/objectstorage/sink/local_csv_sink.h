/**
 * @file base_sink_impl.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-05-30 16:20:22
 */
#pragma once

#include <concurrentqueue/concurrentqueue.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <atomic>
#include <fstream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "concurrentqueue/blockingconcurrentqueue.h"
#include "cppcommon/extends/csv/csv.h"
#include "cppcommon/objectstorage/sink/base_sink.h"

namespace cppcommon::os {

struct CsvWriterOptions {
  std::vector<std::string> headers;
  unsigned int writer_threads_count{std::max(1u, std::thread::hardware_concurrency() / 2)};
};

template <class OutputStream, char Delim>
using CstCSVWriter = csv::DelimWriter<OutputStream, Delim, '"', false>;

static const int64_t kCsvWriterBufferSize = 4 * 1024 * 1024;  // 4MB

using CsvRow = std::vector<std::string>;
template <char Delim>
class CsvWriter : public SinkFileSystem<CsvRow> {
 public:
  explicit CsvWriter(const CsvWriterOptions &options) : options_(&options) {}
  void Open(const std::string &filepath) override {
    filepath_ = filepath;
    ofs_.open(filepath, std::ios::out | std::ios::binary);  // | std::ios::app

    static thread_local std::vector<char> buf(kCsvWriterBufferSize);
    ofs_.rdbuf()->pubsetbuf(buf.data(), buf.size());

    writer_ = csv::make_csv_writer_ptr<Delim, false>(ofs_);
    header_size_ = options_->headers.size();
    if (header_size_) {
      *writer_ << options_->headers;
    }
    // start writer threds
    for (unsigned int i = 0; i < options_->writer_threads_count; ++i) {
      writer_threads_.emplace_back(&CsvWriter::WriteThreadFunc, this);
    }
  }

  inline void WriteThreadFunc() {
    constexpr int kMaxDequeue = 100;
    CsvRow records[kMaxDequeue];
    CsvRow record;
    while (running_ || queue_.size_approx() != 0) {
      queue_.wait_dequeue(record);
      // terminate signal
      if (!running_ && record.empty()) {
        break;
      }
      {
        std::ostringstream oss;
        writer_->WriteTo(oss, record);
        {
          std::lock_guard lock(ofs_mtx_);
          ofs_.write(oss.str().data(), oss.str().size());
        }
      }
    }
  }

  inline int Write(CsvRow &&record) override {
    if (header_size_ && header_size_ != record.size()) {
      spdlog::error("[CsvWriter] unexpected columns size. [header={}, record={}]", header_size_, record.size());
      return 0;
    }
    queue_.enqueue(std::forward<CsvRow>(record));
    return 1;
  }

  bool IsOpen() override { return ofs_.is_open(); }

  void Close() override {
    running_ = false;
    // sending terminate signals
    for (size_t i = 0; i < writer_threads_.size(); ++i) {
      queue_.enqueue({});
    }
    for (auto &th : writer_threads_) {
      if (th.joinable()) th.join();
    }
    if (ofs_) {
      ofs_.flush();
      ofs_.close();
    }
  }

  inline void Flush() override {
    if (ofs_) ofs_.flush();
  }

  static inline bool IsThreadSafe() { return true; }

 protected:
  std::string filepath_;
  std::ofstream ofs_;
  std::mutex ofs_mtx_;
  std::shared_ptr<CstCSVWriter<std::ofstream, Delim>> writer_;
  const CsvWriterOptions *options_{nullptr};
  size_t header_size_{0};
  std::atomic_int in_flight_{0};
  moodycamel::BlockingConcurrentQueue<CsvRow> queue_;
  std::vector<std::thread> writer_threads_;
  bool running_{true};
};

template <char Delim>
using CsvSinkT = BaseSink<CsvRow, CsvWriter<Delim>, CsvWriterOptions>;

using CsvSink = CsvSinkT<','>;
}  // namespace cppcommon::os
