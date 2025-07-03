#include <arrow/api.h>
#include <arrow/filesystem/s3fs.h>
#include <arrow/io/api.h>
#include <arrow/io/type_fwd.h>
#include <arrow/result.h>
#include <arrow/status.h>
#include <arrow/type_fwd.h>
#include <parquet/arrow/writer.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "arrow/record_batch.h"
#include "cppcommon/objectstorage/sink/base_sink.h"
#include "cppcommon/objectstorage/sink/local_arrow_sink.h"
#include "cppcommon/utils/os.h"
#include "gtest/gtest.h"

using namespace cppcommon;
using namespace cppcommon::os;

std::shared_ptr<arrow::RecordBatch> GenRecordBatch() {
  arrow::StringBuilder sb_a;
  arrow::StringBuilder sb_b;

  auto s = sb_a.Append("a_1");
  s = sb_a.Append("a_2");
  s = sb_b.Append("b_1");
  s = sb_b.Append("b_2");

  std::vector<std::shared_ptr<arrow::Array>> arrays;
  arrays.resize(2);
  s = sb_a.Finish(&arrays[0]);
  s = sb_b.Finish(&arrays[1]);

  auto schema = arrow::schema({arrow::field("a", arrow::utf8()), arrow::field("b", arrow::utf8())});
  return arrow::RecordBatch::Make(schema, 2, arrays);
}

std::shared_ptr<arrow::RecordBatch> GenRecordBatchV2() {
  arrow::StringBuilder sb_a;
  arrow::StringBuilder sb_b;

  auto s = sb_a.Append("a2_1");
  s = sb_a.Append("a2_2");
  s = sb_b.Append("b2_1");
  s = sb_b.Append("b2_2");

  std::vector<std::shared_ptr<arrow::Array>> arrays;
  arrays.resize(2);
  s = sb_a.Finish(&arrays[0]);
  s = sb_b.Finish(&arrays[1]);

  auto schema = arrow::schema({arrow::field("a", arrow::utf8()), arrow::field("b2", arrow::utf8())});
  return arrow::RecordBatch::Make(schema, 2, arrays);
}

std::shared_ptr<arrow::Table> GenTable() {
  auto r = arrow::Table::FromRecordBatches({GenRecordBatch()});
  return r.ValueOrDie();
}

std::shared_ptr<arrow::Table> GenTableV2() {
  auto r = arrow::Table::FromRecordBatches({GenRecordBatchV2()});
  return r.ValueOrDie();
}

TEST(Sink, Parquet) {
  auto tb = GenTable();
  spdlog::info("{}", tb->ToString());

  auto out = arrow::io::FileOutputStream::Open("output.parquet").ValueOrDie();
  auto s = parquet::arrow::WriteTable(*tb, arrow::default_memory_pool(), out, 1024);
}

TEST(Sink, ParquetV2) {
  LocalArrowTableSink::Options options{
      .name = "table",
      .name_options{.suffix = "parquet"},
      .roll_options{.is_rotate = false},
  };
  LocalArrowTableSink s(std::move(options));
  s.Write(GenTable());
  s.Write(GenTable());
  // s.Write(GenTableV2()); // different schema is forbidden
}

TEST(Sink, Pmt) {
  LocalArrowTableSink::Options options{
      .name = "table",
      .name_options{.suffix = "parquet"},
      .roll_options{.is_rotate = false},
  };
  LocalArrowTableSink s(std::move(options));
  auto table = GenTable();

  constexpr int kThreadCount = 8;
  constexpr int kWritesPerThread = 10000 * 10;

  auto writer = [&] {
    for (int i = 0; i < kWritesPerThread; ++i) {
      s.Write(table);
    }
  };

  std::vector<std::thread> threads;
  for (int i = 0; i < kThreadCount; ++i) {
    threads.emplace_back(writer);
  }

  for (auto &t : threads) {
    t.join();
  }
}

using arrow::fs::S3FileSystem;
using arrow::fs::S3Options;
TEST(Sink, OSS) {
  auto global_options = arrow::fs::S3GlobalOptions::Defaults();
  auto s = arrow::fs::InitializeS3(global_options);
  auto tb = GenTable();
  spdlog::info("{}", tb->ToString());

  auto ak = GetEnv("OSS_ACCESS_KEY_ID", "");
  auto sk = GetEnv("OSS_ACCESS_KEY_SECRET", "");
  auto region = GetEnv("OSS_REGION", "");
  auto endpoint = GetEnv("OSS_ENDPOINT", "");
  auto bucket = GetEnv("OSS_BUCKET", "");
  spdlog::info("{}, {}, {}, {}, {}", ak, sk, region, endpoint, bucket);

  S3Options options = S3Options::FromAccessKey(ak, sk);
  options.region = region;
  options.endpoint_override = endpoint;
  options.scheme = "http";
  options.force_virtual_addressing = true;

  {
    std::shared_ptr<S3FileSystem> fs = S3FileSystem::Make(options).ValueOrDie();
    auto outpath = fmt::format("{}/output.parquet", bucket);
    auto out = fs->OpenOutputStream(outpath).ValueOrDie();
    auto s = parquet::arrow::WriteTable(*tb, arrow::default_memory_pool(), out, 1024);
    spdlog::info("{}", s.ToString());
  }
}

TEST(Sink, Pmrb) {
  LocalArrowRecordBatchSinkV1::Options options{
      .name = "table",
      .name_options{.suffix = "parquet"},
      .roll_options{.is_rotate = true, .max_rows_per_file = 10000 * 10},
      .on_roll_callback = [](const std::string &fn, auto) { spdlog::info("rollfile: {}", fn); }};
  LocalArrowRecordBatchSinkV1 s(std::move(options));
  auto record = GenRecordBatchV2();

  constexpr int kThreadCount = 8;
  constexpr int kWritesPerThread = 10000 * 5 + 10;

  auto writer = [&] {
    for (int i = 0; i < kWritesPerThread; ++i) {
      s.Write(record);
    }
  };

  std::vector<std::thread> threads;
  for (int i = 0; i < kThreadCount; ++i) {
    threads.emplace_back(writer);
  }

  for (auto &t : threads) {
    t.join();
  }
}

TEST(Sink, PmrbV2) {
  LocalArrowRecordBatchSink::Options options{
      .name = "table",
      .name_options{.suffix = "parquet"},
      .roll_options{.is_rotate = true, .max_rows_per_file = 10000 * 10},
      .on_roll_callback = [](const std::string &fn, auto) { spdlog::info("rollfile: {}", fn); }};
  LocalArrowRecordBatchSink s(std::move(options));
  auto record = GenRecordBatchV2();

  constexpr int kThreadCount = 8;
  constexpr int kWritesPerThread = 10000 * 5 + 10;

  auto writer = [&] {
    for (int i = 0; i < kWritesPerThread; ++i) {
      s.Write(record);
    }
  };

  std::vector<std::thread> threads;
  for (int i = 0; i < kThreadCount; ++i) {
    threads.emplace_back(writer);
  }

  for (auto &t : threads) {
    t.join();
  }
}

TEST(Sink, CsvPm) {
  ArrowCsvLocalSink::Options options{
      .name = "table",
      .name_options{.suffix = "csv"},
      .roll_options{.is_rotate = true, .max_rows_per_file = 10000 * 10},
      .on_roll_callback = [](const std::string &fn, auto) { spdlog::info("rollfile: {}", fn); }};
  ArrowCsvLocalSink s(std::move(options));
  auto record = GenRecordBatchV2();

  constexpr int kThreadCount = 8;
  constexpr int kWritesPerThread = 10000 * 5 + 10;

  auto writer = [&] {
    for (int i = 0; i < kWritesPerThread; ++i) {
      s.Write(record);
    }
  };

  std::vector<std::thread> threads;
  for (int i = 0; i < kThreadCount; ++i) {
    threads.emplace_back(writer);
  }

  for (auto &t : threads) {
    t.join();
  }
}
