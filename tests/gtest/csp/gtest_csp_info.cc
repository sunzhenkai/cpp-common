#include <iostream>

#include "cppcommon/csp/api.h"
#include "gtest/gtest.h"

TEST(CSP, Info) {
  auto r = cppcommon::CloudInfoFactory::Instance().GetInstanceInfo();
  std::cout << r.has_value() << std::endl;
  if (r.has_value()) {
    std::cout << r.value().ToString() << std::endl;
  }
}
