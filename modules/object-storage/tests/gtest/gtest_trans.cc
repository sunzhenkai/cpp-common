#include <alibabacloud/oss/OssClient.h>

#include <cstdlib>
#include <string>

#include "cppcommon/objectstorage/transfor/object_transfor.h"
#include "cppcommon/utils/to_str.h"
#include "gtest/gtest.h"
#include "spdlog/spdlog.h"
using namespace cppcommon::os;
using namespace cppcommon;

TEST(Trans, OSS) {
  AlibabaCloud::OSS::InitializeSdk();

  std::string bucket = std::getenv("OSS_BUCKET");
  auto tr = NewObjectTransfor(ServiceProvider::OSS);
  auto files = tr->List(bucket, "");
  spdlog::info("objects: {}", ToString(files));

  auto pr = tr->Upload(bucket, "test/upload/LICENSE", "LICENSE");
  spdlog::info("upload result: {}", pr.ToString());

  AlibabaCloud::OSS::ShutdownSdk();
}
