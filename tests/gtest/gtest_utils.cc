#include <iostream>

#include "cppcommon/utils/common.h"
#include "gtest/gtest.h"

TEST(Common, Weekday) {
  int64_t ts_ms = 1745377872000;
  std::cout << cppcommon::GetWeekEnd(0, ts_ms) << std::endl;
  ASSERT_EQ(cppcommon::GetWeekEnd(1745378196000, 8), 2);
  ASSERT_EQ(cppcommon::GetWeekEnd(1743264000000, 8), 6);
  ASSERT_EQ(cppcommon::GetWeekEnd(1743264000000, 8), 6);
  ASSERT_EQ(cppcommon::GetWeekEnd(1461340800000, 8), 5);
  ASSERT_EQ(cppcommon::GetWeekEnd(1461340800000, 0), 4);
  ASSERT_EQ(cppcommon::GetWeekEnd(1408291199000, 0), 6);
  ASSERT_EQ(cppcommon::GetWeekEnd(1408291199000, 0), 6);
}
