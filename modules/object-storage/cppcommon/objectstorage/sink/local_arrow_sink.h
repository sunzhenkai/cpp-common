/**
 * @file local_parquet_sink.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-05-30 14:19:34
 */
#pragma once
#include <arrow/api.h>
#include <arrow/csv/api.h>
#include <arrow/filesystem/s3fs.h>
#include <arrow/io/api.h>
#include <arrow/io/type_fwd.h>
#include <arrow/ipc/writer.h>
#include <arrow/record_batch.h>
#include <arrow/result.h>
#include <arrow/status.h>
#include <arrow/type.h>
#include <arrow/type_fwd.h>
#include <parquet/arrow/writer.h>
#include <parquet/properties.h>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "cppcommon/objectstorage/sink/base_sink.h"

namespace cppcommon::os {
template <typename Writer, typename Record>
class ArrowLocalSinkBase : public SinkFileSystem<Record> {
 public:
  void Open(const std::string &filepath) override {
    ofs_ = arrow::io::FileOutputStream::Open(filepath).ValueOrDie();
    filepath_ = filepath;
  }

  bool IsOpen() override { return static_cast<bool>(ofs_); }

  void Close() override {
    if (ofs_) {
      Flush();
      if (writer_) {
        auto s = writer_->Close();
        writer_.reset();
        if (!s.ok()) {
          spdlog::error("[ArrowLocalSinkBase] close file failed. [filepath={}]", filepath_);
        }
      }
      /* ofs_ */ {
        auto s = ofs_->Close();
        ofs_.reset();
        if (!s.ok()) {
          spdlog::error("[ArrowLocalSinkBase] close file failed. [filepath={}]", filepath_);
        } else {
          spdlog::info("[ArrowLocalSinkBase] close file success. [filepath={}]", filepath_);
        }
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

 protected:
  std::shared_ptr<arrow::io::FileOutputStream> ofs_;
  std::shared_ptr<Writer> writer_;
  std::string filepath_;
};

class ArrowTableParquetWriter : public ArrowLocalSinkBase<parquet::arrow::FileWriter, std::shared_ptr<arrow::Table>> {
 public:
  inline int Write(std::shared_ptr<arrow::Table> &&record) override {
    if (!writer_) {
      std::shared_ptr<parquet::WriterProperties> props = parquet::WriterProperties::Builder().build();
      // .compression(arrow::Compression::SNAPPY)
      std::shared_ptr<parquet::ArrowWriterProperties> arrow_props =
          parquet::ArrowWriterProperties::Builder().set_use_threads(false)->build();
      writer_ = parquet::arrow::FileWriter::Open(*record->schema().get(), arrow::default_memory_pool(), ofs_, props,
                                                 arrow_props)
                    .ValueOrDie();
    }
    auto s = writer_->WriteTable(*record);
    if (s.ok()) {
      return record->num_rows();
    } else {
      spdlog::error("write arrow::Table failed. [error={}]", s.ToString());
      return 0;
    }
  }
};

class ArrowParquetWriter : public ArrowLocalSinkBase<parquet::arrow::FileWriter, std::shared_ptr<arrow::RecordBatch>> {
 public:
  inline int Write(std::shared_ptr<arrow::RecordBatch> &&record) override {
    if (!ofs_) {
      spdlog::error("write arrow::RecordBatch failed, file stream not ready.");
      return 0;
    }
    if (!writer_) {
      std::shared_ptr<parquet::WriterProperties> props =
          parquet::WriterProperties::Builder().max_row_group_length(1024 * 10)->build();
      // .compression(arrow::Compression::SNAPPY)
      std::shared_ptr<parquet::ArrowWriterProperties> arrow_props =
          parquet::ArrowWriterProperties::Builder().set_use_threads(false)->build();
      writer_ = parquet::arrow::FileWriter::Open(*record->schema().get(), arrow::default_memory_pool(), ofs_, props,
                                                 arrow_props)
                    .ValueOrDie();
    }
    auto s = writer_->WriteRecordBatch(*record);
    if (s.ok()) {
      return record->num_rows();
    } else {
      spdlog::error("write arrow::RecordBatch failed. [error={}]", s.ToString());
      return 0;
    }
  }
};

class ArrowParquetWriterV2
    : public ArrowLocalSinkBase<parquet::arrow::FileWriter, std::shared_ptr<arrow::RecordBatch>> {
 public:
  inline int Write(std::shared_ptr<arrow::RecordBatch> &&record) override {
    auto count = record->num_rows();
    records_.emplace_back(std::move(record));
    return count;
  }

  void Close() override {
    if (!records_.empty()) {
      auto record = records_.front();
      std::shared_ptr<parquet::WriterProperties> props = parquet::WriterProperties::Builder().build();
      // .compression(arrow::Compression::SNAPPY)
      std::shared_ptr<parquet::ArrowWriterProperties> arrow_props =
          parquet::ArrowWriterProperties::Builder().set_use_threads(false)->build();
      writer_ = parquet::arrow::FileWriter::Open(*record->schema().get(), arrow::default_memory_pool(), ofs_, props,
                                                 arrow_props)
                    .ValueOrDie();

      auto maybe_table = arrow::Table::FromRecordBatches(records_);
      if (!maybe_table.ok()) {
        spdlog::error("sink arrow RecordBatch failed: {}", maybe_table.status().ToString());
      } else {
        std::shared_ptr<arrow::Table> table = maybe_table.ValueOrDie();
        auto s = writer_->WriteTable(*table);
        if (!s.ok()) {
          spdlog::error("write arrow::Table failed. [error={}]", s.ToString());
        }
        table.reset();
      }
      records_.clear();
    }

    ArrowLocalSinkBase::Close();
  }

 private:
  std::vector<std::shared_ptr<arrow::RecordBatch>> records_;
};

class ArrowCsvWriter : public ArrowLocalSinkBase<arrow::ipc::RecordBatchWriter, std::shared_ptr<arrow::RecordBatch>> {
 public:
  inline int Write(std::shared_ptr<arrow::RecordBatch> &&record) override {
    if (!ofs_) {
      spdlog::error("write arrow::RecordBatch failed, file stream not ready.");
      return 0;
    }
    if (!writer_) {
      auto ops = arrow::csv::WriteOptions::Defaults();
      writer_ = arrow::csv::MakeCSVWriter(ofs_, record->schema(), ops).ValueOrDie();
    }
    auto s = writer_->WriteRecordBatch(*record);
    if (s.ok()) {
      return record->num_rows();
    } else {
      spdlog::error("write arrow::RecordBatch failed. [error={}]", s.ToString());
      return 0;
    }
  }
};

using LocalArrowTableSink = BaseSink<std::shared_ptr<arrow::Table>, ArrowTableParquetWriter>;
using LocalArrowRecordBatchSinkV1 = BaseSink<std::shared_ptr<arrow::RecordBatch>, ArrowParquetWriter>;
using LocalArrowRecordBatchSink = BaseSink<std::shared_ptr<arrow::RecordBatch>, ArrowParquetWriterV2>;
using ArrowCsvLocalSink = BaseSink<std::shared_ptr<arrow::RecordBatch>, ArrowCsvWriter>;
}  // namespace cppcommon::os
