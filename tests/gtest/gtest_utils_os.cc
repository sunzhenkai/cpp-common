#include <iostream>

#include "cppcommon/utils/os.h"
#include "gtest/gtest.h"

TEST(UtilsOs, env) {
  // std::cout << cppcommon::GetEnv("PATH", "NONE") << std::endl;
  // std::cout << cppcommon::GetEnv("NON_EXISTS_PATH", "NONE") << std::endl;
  ASSERT_NE("NONE", cppcommon::GetEnv("PATH", "NONE"));
  ASSERT_EQ("NONE", cppcommon::GetEnv("NON_EXISTS_PATH", "NONE"));
}

TEST(OS, DateFileName) {
  auto s = cppcommon::GetDatedFilePath("runtime.og", "log-bucket/service");
  std::cout << s << std::endl;
  s = cppcommon::GetDatedFilePath("runtime.og", "log-bucket/service", 8);
  std::cout << s << std::endl;
}
