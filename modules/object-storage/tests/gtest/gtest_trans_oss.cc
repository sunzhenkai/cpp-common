#include <string>

#include "absl/status/statusor.h"
#include "cppcommon/objectstorage/transfor/api.h"
#include "gtest/gtest.h"

TEST(Trans, OssList) {
  std::string bucket = std::getenv("OSS_BUCKET");
  auto tr = NewObjectTransfor(cppcommon::os::ServiceProvider::OSS);
  auto r = tr->List(bucket, "");
}
