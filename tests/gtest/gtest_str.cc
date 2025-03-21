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
