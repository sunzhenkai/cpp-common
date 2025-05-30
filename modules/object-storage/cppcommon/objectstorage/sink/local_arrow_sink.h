/**
 * @file local_parquet_sink.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-05-30 14:19:34
 */
#pragma once
#include <arrow/api.h>
#include <arrow/filesystem/s3fs.h>
#include <arrow/io/api.h>
#include <arrow/io/type_fwd.h>
#include <arrow/result.h>
#include <arrow/status.h>
#include <arrow/type_fwd.h>
#include <parquet/arrow/writer.h>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <memory>
#include <string>

#include "cppcommon/objectstorage/sink/base_sink.h"

namespace cppcommon::os {
template <typename Record>
class LocalArrowSinkFileSystem : public SinkFileSystem<Record> {
 public:
  void Open(const std::string &filepath) override {
    ofs_ = arrow::io::FileOutputStream::Open(filepath).ValueOrDie();
    filepath_ = filepath;
  }

  bool IsOpen() override { return static_cast<bool>(ofs_); }

  void Close() override {
    if (ofs_) {
      auto s = ofs_->Close();
      if (!s.ok()) {
        spdlog::error("[LocalArrowParquetSinkFileSystem] close file failed. [filepath={}]", filepath_);
      }
    }
  }

  inline void Flush() override {
    if (ofs_) {
      auto s = ofs_->Flush();
      if (!s.ok()) {
        spdlog::error("[LocalArrowParquetSinkFileSystem] flush failed. [filepath={}]", filepath_);
      }
    }
  }

  static bool IsExists(const std::string &filepath) { return std::filesystem::exists(filepath); }

 protected:
  std::shared_ptr<arrow::io::FileOutputStream> ofs_;
  std::string filepath_;
};

class LocalArrowParquetSinkFileSystem : public LocalArrowSinkFileSystem<std::shared_ptr<arrow::Table>> {
 public:
  inline int Write(std::shared_ptr<arrow::Table> &&record) override {
    auto s = parquet::arrow::WriteTable(*record, arrow::default_memory_pool(), ofs_, 1024);
    if (s.ok()) {
      return record->num_rows();
    } else {
      return 0;
    }
  }
};

using LocalArrowTableSink = BaseSink<std::shared_ptr<arrow::Table>, LocalArrowParquetSinkFileSystem>;
}  // namespace cppcommon::os
