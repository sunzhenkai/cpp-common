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

#include <filesystem>
#include <memory>

#include "arrow/api.h"
#include "arrow/filesystem/api.h"
#include "cppcommon/objectstorage/sink/base_sink.h"

namespace cppcommon {
template <typename Record>
class LocalArrowSinkFileSystem : public SinkFileSystem<Record> {
 public:
  void Open(const std::string &filepath) override { ofs_ = arrow::io::FileOutputStream::Open(filepath).ValueOrDie(); }
  bool IsOpen() override { return bool(ofs_); }

  void Close() override {
    if (ofs_) ofs_->Close();
  }

  inline void Flush() override {
    if (ofs_) ofs_->Flush();
  }

  static bool IsExists(const std::string &filepath) { return std::filesystem::exists(filepath); }

 protected:
  std::shared_ptr<arrow::io::FileOutputStream> ofs_;
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
}  // namespace cppcommon
