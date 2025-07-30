/**
 * @file base_sink_impl.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-05-30 16:20:22
 */
#pragma once

#include <spdlog/spdlog.h>

#include <fstream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

#include "cppcommon/extends/csv/csv.h"
#include "cppcommon/objectstorage/sink/base_sink.h"

namespace cppcommon::os {

struct CsvWriterOptions {
  std::vector<std::string> headers;
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

    // do not use csv writer to write things
    writer_ = csv::make_csv_writer_ptr<Delim, false>(fake_ofs_);
    header_size_ = options_->headers.size();
    if (header_size_) {
      Write(options_->headers);
    }
  }

  inline int Write(CsvRow &&record) override {
    if (header_size_ && header_size_ != record.size()) {
      spdlog::error("[CsvWriter] unexpected columns size. [header={}, record={}]", header_size_, record.size());
      return 0;
    }
    std::ostringstream oss;
    writer_->WriteTo(oss, record);
    {
      std::lock_guard lock(ofs_mtx_);
      ofs_.write(oss.str().data(), oss.str().size());
    }
    return 1;
  }

  bool IsOpen() override { return ofs_.is_open(); }

  void Close() override {
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
  std::ostringstream fake_ofs_;
  std::mutex ofs_mtx_;
  std::shared_ptr<CstCSVWriter<std::ofstream, Delim>> writer_;
  const CsvWriterOptions *options_{nullptr};
  size_t header_size_{0};
};

template <char Delim>
using CsvSinkT = BaseSink<CsvRow, CsvWriter<Delim>, CsvWriterOptions>;

using CsvSink = CsvSinkT<','>;
}  // namespace cppcommon::os
