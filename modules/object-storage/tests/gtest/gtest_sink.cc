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
#include <utility>
#include <vector>

#include "arrow/builder.h"
#include "arrow/record_batch.h"
#include "cppcommon/objectstorage/api.h"
#include "cppcommon/utils/os.h"
#include "gtest/gtest.h"

std::shared_ptr<arrow::RecordBatch> GenRecordBatch() {
  arrow::StringBuilder sb_a;
  arrow::StringBuilder sb_b;

  sb_a.Append("a_1");
  sb_a.Append("a_2");
  sb_b.Append("b_1");
  sb_b.Append("b_2");

  std::vector<std::shared_ptr<arrow::Array>> arrays;
  arrays.resize(2);
  sb_a.Finish(&arrays[0]);
  sb_b.Finish(&arrays[1]);

  auto schema = arrow::schema({arrow::field("a", arrow::utf8()), arrow::field("b", arrow::utf8())});
  return arrow::RecordBatch::Make(schema, 2, arrays);
}

std::shared_ptr<arrow::Table> GenTable() {
  auto r = arrow::Table::FromRecordBatches({GenRecordBatch()});
  return r.ValueOrDie();
}

TEST(Sink, Parquet) {
  auto tb = GenTable();
  spdlog::info("{}", tb->ToString());

  auto out = arrow::io::FileOutputStream::Open("output.parquet").ValueOrDie();
  parquet::arrow::WriteTable(*tb, arrow::default_memory_pool(), out, 1024);
}

TEST(Sink, ParquetV2) {
  cppcommon::LocalArrowTableSink ::Options options{.name = "table", .is_rotate = false, .suffix = "parquet"};
  cppcommon::LocalArrowTableSink s(std::move(options));
  auto tb = GenTable();
  s.Write(tb);
}

using arrow::fs::S3FileSystem;
using arrow::fs::S3Options;
TEST(Sink, OSS) {
  auto global_options = arrow::fs::S3GlobalOptions::Defaults();
  arrow::fs::InitializeS3(global_options);
  auto tb = GenTable();
  spdlog::info("{}", tb->ToString());

  auto ak = cppcommon::GetEnv("OSS_ACCESS_KEY_ID", "");
  auto sk = cppcommon::GetEnv("OSS_ACCESS_KEY_SECRET", "");
  auto region = cppcommon::GetEnv("OSS_REGION", "");
  auto endpoint = cppcommon::GetEnv("OSS_ENDPOINT", "");
  auto bucket = cppcommon::GetEnv("OSS_BUCKET", "");
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
    parquet::arrow::WriteTable(*tb, arrow::default_memory_pool(), out, 1024);
  }
}
