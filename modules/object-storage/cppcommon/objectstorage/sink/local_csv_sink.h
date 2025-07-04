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
#include <string>
#include <utility>
#include <vector>

#include "cppcommon/extends/csv/csv.h"
#include "cppcommon/objectstorage/sink/base_sink.h"

namespace cppcommon::os {

struct CsvWriterOptions {
  std::vector<std::string> headers;
};

using CsvRow = std::vector<std::string>;
class CsvWriter : public SinkFileSystem<CsvRow> {
 public:
  explicit CsvWriter(const CsvWriterOptions &options) : options_(&options) {}
  void Open(const std::string &filepath) override {
    filepath_ = filepath;
    ofs_.open(filepath, std::ios::out | std::ios::app);
    writer_ = csv::make_csv_writer_ptr(ofs_);
    header_size_ = options_->headers.size();
    if (header_size_) {
      *writer_ << options_->headers;
    }
  }

  inline int Write(CsvRow &&record) override {
    if (header_size_ && header_size_ != record.size()) {
      spdlog::error("[CsvWriter] unexpected columns size. [header={}, record={}]", header_size_, record.size());
      return 0;
    }
    *writer_ << record;
    return 1;
  }

  bool IsOpen() override { return ofs_.is_open(); }

  void Close() override {
    if (ofs_) ofs_.close();
  }

  inline void Flush() override {
    if (ofs_) ofs_.flush();
  }

 protected:
  std::string filepath_;
  std::ofstream ofs_;
  std::shared_ptr<csv::CSVWriter<std::ofstream>> writer_;
  const CsvWriterOptions *options_{nullptr};
  size_t header_size_{0};
};

using CsvSink = BaseSink<CsvRow, CsvWriter, CsvWriterOptions>;
}  // namespace cppcommon::os
