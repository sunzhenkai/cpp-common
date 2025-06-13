#include <iostream>

#include "cppcommon/utils/time.h"
#include "gtest/gtest.h"

TEST(Time, Weekday) {
  int64_t ts_ms = 1745377872000;
  std::cout << cppcommon::GetWeekDay(0, ts_ms) << std::endl;
  ASSERT_EQ(cppcommon::GetWeekDay(1745378196000, 8), 2);
  ASSERT_EQ(cppcommon::GetWeekDay(1743264000000, 8), 6);
  ASSERT_EQ(cppcommon::GetWeekDay(1743264000000, 8), 6);
  ASSERT_EQ(cppcommon::GetWeekDay(1461340800000, 8), 5);
  ASSERT_EQ(cppcommon::GetWeekDay(1461340800000, 0), 4);
  ASSERT_EQ(cppcommon::GetWeekDay(1408291199000, 0), 6);
  ASSERT_EQ(cppcommon::GetWeekDay(1408291199000, 0), 6);
}

TEST(Time, Apply) {
  auto di = cppcommon::GetDateInfo(1408291199000);
  std::cout << "Year=" << di.GetHumanYear() << std::endl;
  // 2014-08-17 15-59-59
  std::cout << di.Format("%Y-%m-%d %H-%M-%S") << std::endl;
  auto ndi = di.Apply(0, 0, 0, -1, 0, 0);
  std::cout << ndi.Format("%Y-%m-%d %H-%M-%S") << std::endl;
  ASSERT_EQ(di.GetHour(), ndi.GetHour() + 1);

  ndi = di.Apply(0, 0, -17, 0, 0, 0);
  std::cout << ndi.Format("%Y-%m-%d %H-%M-%S") << std::endl;
  ASSERT_EQ(ndi.GetMonthDay(), 31);

  ndi = di.Apply(0, 0, 0, -16, 0, 0);
  std::cout << ndi.Format("%Y-%m-%d %H-%M-%S") << std::endl;
  ASSERT_EQ(ndi.GetMonthDay(), 16);
  ASSERT_EQ(ndi.GetHour(), 23);

  ndi = di.Apply(0, -8, 0, 0, 0, 0);
  std::cout << ndi.Format("%Y-%m-%d %H-%M-%S") << std::endl;
  ASSERT_EQ(ndi.GetMonth(), 11);  // 0~11

  ndi = di.Apply(0, -7, 0, 0, 0, 0);
  std::cout << ndi.Format("%Y-%m-%d %H-%M-%S") << std::endl;
  ASSERT_EQ(ndi.GetMonth(), 0);  // 0~11

  // 2014-01-17 15-59-59
  std::cout << ndi.Format("%y%m") << std::endl;
  ASSERT_EQ(ndi.Format("%y%m"), "1401");
}

TEST(Time, Fmt) {
  auto di = cppcommon::GetDateInfo(1749800920000);
  auto f = di.Format("%Y-%m-%d %H:%M:%S");
  std::cout << f << std::endl;
  ASSERT_EQ("2025-06-13 07:48:40", f);
}
