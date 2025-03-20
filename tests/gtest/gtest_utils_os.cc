#include "cppcommon/utils/os.h"
#include "gtest/gtest.h"

TEST(UtilsOs, env) {
  // std::cout << cppcommon::GetEnv("PATH", "NONE") << std::endl;
  // std::cout << cppcommon::GetEnv("NON_EXISTS_PATH", "NONE") << std::endl;
  ASSERT_NE("NONE", cppcommon::GetEnv("PATH", "NONE"));
  ASSERT_EQ("NONE", cppcommon::GetEnv("NON_EXISTS_PATH", "NONE"));
}
