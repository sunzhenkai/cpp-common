#include <cstdlib>

#include "cppcommon/objectstorage/trans/base_trans.h"
#include "gtest/gtest.h"

using namespace cppcommon::os;
using namespace cppcommon;
TEST(Trans, OSS) {
  Aws::SDKOptions options;
  Aws::InitAPI(options);

  ObjectStorageTransfor tr(StorageService::OSS);
  tr.List(std::getenv("OSS_BUCKET"), "");

  Aws::ShutdownAPI(options);
}
