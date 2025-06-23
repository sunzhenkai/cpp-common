#include <spdlog/spdlog.h>

#include <string>

#include "absl/status/statusor.h"
#include "cppcommon/io/file/rw.h"
#include "cppcommon/objectstorage/transfor/api.h"
#include "cppcommon/objectstorage/transfor/storage_provider.h"
#include "cppcommon/utils/to_str.h"
#include "gtest/gtest.h"

TEST(Trans, Gcs) {
  std::string cred_json = std::getenv("GCS_SERVICE_ACCOUNT_JSON");
  std::string bucket = std::getenv("GCS_BUCKET");
  auto service_account_json = cppcommon::ReadFile(cred_json.data());

  auto tr = NewObjectTransfor(cppcommon::os::ServiceProvider::GCS, service_account_json);

  // list
  auto r = tr->List(bucket, "test");
  spdlog::info("bucket={}, objects={}", bucket, cppcommon::ToString(r.value()));

  // write
  cppcommon::WriteFile("foo", "bar");
  auto s = tr->Upload({.bucket = bucket, .remote_file_path = "test/foo", .local_file_path = "foo"});
  spdlog::info("upload result: {}", s.ToString());
  s = tr->Upload({.bucket = bucket, .remote_file_path = "test/sub/foo", .local_file_path = "foo"});
  spdlog::info("upload result: {}", s.ToString());

  // download file
  s = tr->DownloadFile({.bucket = bucket, .remote_file_path = "test/foo", .local_file_path = "output/foo-download"});
  spdlog::info("download file result: {}", s.ToString());

  // download
  cppcommon::os::TransferMeta m{.bucket = bucket, .remote_file_path = "test", .local_file_path = "output/"};
  auto r2 = tr->Download(m);
  if (r2.ok()) {
    spdlog::info("download success", m.ToString());
    for (auto &p : r2.value()) {
      spdlog::info("\tfile: {}", p.string());
    }
  } else {
    spdlog::error("download failed, error={}", r2.status().ToString());
  }
}

TEST(Trans, RemovePrefix) {
  std::string r;
  std::string e = "path/to/object";
  r = cppcommon::os::TryRemoveCloudStoragePrefix(cppcommon::os::ServiceProvider::GCS, "test",
                                                 "gs://test/path/to/object");
  spdlog::info("{}", r);
  ASSERT_EQ(r, e);

  r = cppcommon::os::TryRemoveCloudStoragePrefix(cppcommon::os::ServiceProvider::GCS, "test", "/path/to/object");
  spdlog::info("{}", r);
  ASSERT_EQ(r, e);

  r = cppcommon::os::TryRemoveCloudStoragePrefix(cppcommon::os::ServiceProvider::GCS, "test", "path/to/object");
  spdlog::info("{}", r);
  ASSERT_EQ(r, e);

  r = cppcommon::os::TryRemoveCloudStoragePrefix(cppcommon::os::ServiceProvider::GCS, "path", "path/to/object");
  spdlog::info("{}", r);
  ASSERT_EQ(r, e);
}

TEST(Trans, GcsV2) {
  auto tr = NewObjectTransfor(cppcommon::os::ServiceProvider::GCS);
  auto bucket = "gcp-spark-ml-train-new";
  auto r = tr->List(bucket, "liguoyu/data/models/ftrl/model_duf_outer_v2");
  spdlog::info("bucket={}, objects={}", bucket, cppcommon::ToString(r.value()));

  // auto s = tr->DownloadFile({.bucket = bucket, .remote_file_path =
  // "liguoyu/data/models/ftrl/model_duf_outer_v2/online_model_2025-05-31", .local_file_path = "output/foo-download"});
  auto p = "gcp-spark-ml-train-new/liguoyu/data/models/ftrl/model_duf_outer_v1/online_model_2025-05-31";
  auto s = tr->DownloadFile({.bucket = bucket, .remote_file_path = p, .local_file_path = "output/foo-download"});
  spdlog::info("{}", s.ToString());
}
