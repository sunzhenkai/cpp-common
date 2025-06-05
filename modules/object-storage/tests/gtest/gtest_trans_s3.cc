#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <spdlog/spdlog.h>

#include <string>

#include "absl/status/statusor.h"
#include "cppcommon/io/file/rw.h"
#include "cppcommon/objectstorage/transfor/api.h"
#include "cppcommon/objectstorage/transfor/storage_provider.h"
#include "cppcommon/utils/to_str.h"
#include "gtest/gtest.h"

TEST(Trans, S3) {
  Aws::SDKOptions options;
  Aws::InitAPI(options);

  std::string bucket = std::getenv("S3_BUCKET");
  auto tr = NewObjectTransfor(cppcommon::os::ServiceProvider::S3);
  auto r = tr->List(bucket, "test");
  spdlog::info("bucket={}, objects={}", bucket, cppcommon::ToString(r.value()));

  cppcommon::WriteFile("foo", "bar");
  auto s = tr->Upload({.bucket = bucket, .remote_file_path = "test/foo", .local_file_path = "foo"});
  spdlog::info("upload result: {}", s.ToString());
  s = tr->Upload({.bucket = bucket, .remote_file_path = "test/sub/foo", .local_file_path = "foo"});
  spdlog::info("upload result: {}", s.ToString());

  s = tr->DownloadFile({.bucket = bucket, .remote_file_path = "test/foo", .local_file_path = "output/foo-download"});
  spdlog::info("download file result: {}", s.ToString());

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

  Aws::ShutdownAPI(options);
}
