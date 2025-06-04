#include <alibabacloud/oss/OssClient.h>

#include <cstdlib>
#include <memory>
#include <string>
#include <utility>

#include "cppcommon/objectstorage/sink/base_sink.h"
#include "cppcommon/objectstorage/sink/local_arrow_sink.h"
#include "cppcommon/objectstorage/sink/local_text_sink.h"
#include "cppcommon/objectstorage/transfor/object_transfor.h"
#include "cppcommon/utils/to_str.h"
#include "gtest/gtest.h"
#include "spdlog/spdlog.h"
using namespace cppcommon::os;
using namespace cppcommon;

TEST(Trans, OSS) {
  std::string bucket = std::getenv("OSS_BUCKET");
  auto tr = NewObjectTransfor(ServiceProvider::OSS);
  auto files = tr->List(bucket, "");
  spdlog::info("objects: {}", ToString(files.value()));

  auto pr = tr->Upload({bucket, "test/upload/LICENSE", "LICENSE"});
  spdlog::info("upload result: {}", pr.ToString());
}

TEST(Trans, Log) {
  AlibabaCloud::OSS::InitializeSdk();
  std::string bucket = std::getenv("OSS_BUCKET");
  auto tr = NewObjectTransfor(ServiceProvider::OSS);

  LocalBasicSink::Options options{
      .name = "runtime", .is_rotate = true, .max_rows_per_file = 2, .on_roll_callback = [&](const std::string &fn) {
        spdlog::info("rollfile: {}", fn);
        auto pr = tr->Upload({bucket, "test/upload/abc", fn});
        spdlog::info("upload result: {}", pr.ToString());
        pr = tr->Upload({bucket, fmt::format("test/upload/{}", fn), fn});
        spdlog::info("upload result: {}", pr.ToString());
      }};
  LocalBasicSink s(std::move(options));
  s.Write("a");
  s.Write("b");
  s.Write("c");

  AlibabaCloud::OSS::ShutdownSdk();
}

std::shared_ptr<arrow::Table> GenTable();

TEST(Trans, Arrow) {
  LocalArrowTableSink::Options options{.name = "table", .is_rotate = true, .max_rows_per_file = 2, .suffix = "parquet"};
  LocalArrowTableSink s(std::move(options));
  s.Write(GenTable());
  s.Write(GenTable());
}
