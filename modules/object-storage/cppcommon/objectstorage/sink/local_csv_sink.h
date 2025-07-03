/**
 * @file base_sink_impl.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-05-30 16:20:22
 */
#pragma once

#include <fstream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "cppcommon/extends/csv/csv.hpp"
#include "cppcommon/objectstorage/sink/base_sink.h"

namespace cppcommon::os {

using CsvRow = std::vector<std::string>;
class CsvWriter : public SinkFileSystem<CsvRow> {
 public:
  void Open(const std::string &filepath) override {
    filepath_ = filepath;
    ofs_.open(filepath, std::ios::out | std::ios::app);
    writer_ = csv::make_csv_writer_ptr(ofs_);
  }

  inline int Write(CsvRow &&record) override {
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
};

using CsvSink = BaseSink<CsvRow, CsvWriter>;
}  // namespace cppcommon::os
