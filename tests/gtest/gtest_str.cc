#include <string>
#include <string_view>

#include "cppcommon/utils/str.h"
#include "gtest/gtest.h"

TEST(Str, Split) {
  std::string s;
  std::vector<std::string> r;
  char delimeter = '.';

  // case 0
  s = ".a.b.c";
  cppcommon::StringSplit(r, s, delimeter);
  std::cout << cppcommon::ToString(r) << std::endl;
  cppcommon::StringSplit(r, s, delimeter, true);
  std::cout << cppcommon::ToString(r) << std::endl;

  // case 1
  s = ".a.baaa.c";
  cppcommon::StringSplit(r, s, delimeter);
  std::cout << cppcommon::ToString(r) << std::endl;
  cppcommon::StringSplit(r, s, delimeter, true);
  std::cout << cppcommon::ToString(r) << std::endl;

  // case 2
  s = "";
  cppcommon::StringSplit(r, s, delimeter);
  std::cout << cppcommon::ToString(r) << std::endl;
  cppcommon::StringSplit(r, s, delimeter, true);
  std::cout << cppcommon::ToString(r) << std::endl;

  // case 3
  std::string_view sv;
  sv = "a.b.c";
  cppcommon::StringSplit(r, sv, delimeter);
  std::cout << cppcommon::ToString(r) << std::endl;
  cppcommon::StringSplit(r, sv, delimeter, true);
  std::cout << cppcommon::ToString(r) << std::endl;

  // case 4
  std::vector<std::string_view> rsv;
  sv = ".";
  cppcommon::StringSplit(rsv, sv, delimeter);
  std::cout << cppcommon::ToString(rsv) << std::endl;
  cppcommon::StringSplit(rsv, sv, delimeter, true);
  std::cout << cppcommon::ToString(rsv) << std::endl;

  // case 5
  s = "..";
  cppcommon::StringSplit(r, s, delimeter);
  std::cout << cppcommon::ToString(r) << std::endl;
  cppcommon::StringSplit(r, s, delimeter, true);
  std::cout << cppcommon::ToString(r) << std::endl;

  // case 6
  s = "a...b";
  cppcommon::StringSplit(r, s, delimeter);
  std::cout << cppcommon::ToString(r) << std::endl;
  cppcommon::StringSplit(r, s, delimeter, true);
  std::cout << cppcommon::ToString(r) << std::endl;

  // case 7
  s = ".";
  cppcommon::StringSplit(r, s, delimeter);
  std::cout << cppcommon::ToString(r) << std::endl;
  cppcommon::StringSplit(r, s, delimeter, true);
  std::cout << cppcommon::ToString(r) << std::endl;
}

TEST(Str, SplitWithFilter) {
  std::string s;
  std::vector<std::string> r;

  // case 1
  s = "aa.bb..cc";
  cppcommon::StrSplitWithFilter(r, s, '.', "bb"sv, true);
  std::cout << cppcommon::ToString(r) << std::endl;
  ASSERT_EQ(r.size(), 2);
  ASSERT_EQ(r[0], "aa");
  ASSERT_EQ(r[1], "cc");

  s = "bb";
  cppcommon::StrSplitWithFilter(r, s, '.', "bb"sv);
  std::cout << cppcommon::ToString(r) << std::endl;
  ASSERT_EQ(r.size(), 0);

  s = ".bb.";
  cppcommon::StrSplitWithFilter(r, s, '.', "bb"sv);
  std::cout << cppcommon::ToString(r) << std::endl;
  ASSERT_EQ(r.size(), 2);
  ASSERT_EQ(r[0], "");
  ASSERT_EQ(r[1], "");

  s = "a.bbb.c";
  cppcommon::StrSplitWithFilter(r, s, '.', "bb"sv);
  std::cout << cppcommon::ToString(r) << std::endl;
  ASSERT_EQ(r.size(), 3);
  ASSERT_EQ(r[0], "a");
  ASSERT_EQ(r[1], "bbb");
  ASSERT_EQ(r[2], "c");

  s = "..";
  cppcommon::StrSplitWithFilter(r, s, '.', "bb"sv);
  std::cout << cppcommon::ToString(r) << std::endl;
  ASSERT_EQ(r.size(), 3);
  ASSERT_EQ(r[0], "");
  ASSERT_EQ(r[1], "");
  ASSERT_EQ(r[2], "");

  s = "";
  cppcommon::StrSplitWithFilter(r, s, '.', "bb"sv);
  std::cout << cppcommon::ToString(r) << std::endl;
  ASSERT_EQ(r.size(), 1);
  ASSERT_EQ(r[0], "");

  s = "";
  cppcommon::StrSplitWithFilter(r, s, '.', "bb"sv, true);
  std::cout << cppcommon::ToString(r) << std::endl;
  ASSERT_EQ(r.size(), 0);
}
