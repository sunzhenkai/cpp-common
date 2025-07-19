#include <iostream>

#include "cppcommon/os/machine_info.h"
#include "gtest/gtest.h"

TEST(OsMI, Static) {
  cppcommmon::MachineInfo mi;
  std::cout << mi.ToString() << std::endl;
}
