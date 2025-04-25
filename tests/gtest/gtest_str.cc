#include <spdlog/spdlog.h>

#include <string>
#include <string_view>
#include <unordered_map>

#include "cppcommon/utils/str.h"
#include "cppcommon/utils/to_str.h"
#include "gtest/gtest.h"

TEST(StringSplit, Split) {
  std::string s;
  std::vector<std::string> r;
  char delimeter = '.';

  // case 0
  s = ".a.b.c";
  cppcommon::StringSplit(r, s, delimeter);
  std::cout << cppcommon::ToString(r) << std::endl;
  ASSERT_EQ(r.size(), 4);
  ASSERT_EQ(r[0], "");
  cppcommon::StringSplit(r, s, delimeter, true);
  ASSERT_EQ(r.size(), 3);
  ASSERT_EQ(r[0], "a");
  std::cout << cppcommon::ToString(r) << std::endl;

  // case 1
  s = ".a.baaa.c";
  cppcommon::StringSplit(r, s, delimeter);
  ASSERT_EQ(r.size(), 4);
  ASSERT_EQ(r[2], "baaa");

  std::cout << cppcommon::ToString(r) << std::endl;
  cppcommon::StringSplit(r, s, delimeter, true);
  ASSERT_EQ(r.size(), 3);
  ASSERT_EQ(r[1], "baaa");
  std::cout << cppcommon::ToString(r) << std::endl;

  // case 2
  s = "";
  cppcommon::StringSplit(r, s, delimeter);
  ASSERT_EQ(r.size(), 1);
  std::cout << cppcommon::ToString(r) << std::endl;
  cppcommon::StringSplit(r, s, delimeter, true);
  ASSERT_EQ(r.size(), 0);
  std::cout << cppcommon::ToString(r) << std::endl;

  // case 3
  std::string_view sv;
  sv = "a.b.c";
  cppcommon::StringSplit(r, sv, delimeter);
  ASSERT_EQ(r.size(), 3);
  ASSERT_EQ(r[2], "c");
  std::cout << cppcommon::ToString(r) << std::endl;
  cppcommon::StringSplit(r, sv, delimeter, true);
  ASSERT_EQ(r.size(), 3);
  ASSERT_EQ(r[2], "c");
  std::cout << cppcommon::ToString(r) << std::endl;

  // case 4
  std::vector<std::string_view> rsv;
  sv = ".";
  cppcommon::StringSplit(rsv, sv, delimeter);
  ASSERT_EQ(rsv.size(), 2);
  ASSERT_EQ(rsv[1], "");
  std::cout << cppcommon::ToString(rsv) << std::endl;
  cppcommon::StringSplit(rsv, sv, delimeter, true);
  ASSERT_EQ(rsv.size(), 0);
  std::cout << cppcommon::ToString(rsv) << std::endl;

  // case 5
  s = "..";
  cppcommon::StringSplit(r, s, delimeter);
  std::cout << cppcommon::ToString(r) << std::endl;
  ASSERT_EQ(r.size(), 3);
  cppcommon::StringSplit(r, s, delimeter, true);
  std::cout << cppcommon::ToString(r) << std::endl;
  ASSERT_EQ(r.size(), 0);

  // case 6
  s = "a...b";
  cppcommon::StringSplit(r, s, delimeter);
  ASSERT_EQ(r.size(), 4);
  std::cout << cppcommon::ToString(r) << std::endl;
  cppcommon::StringSplit(r, s, delimeter, true);
  ASSERT_EQ(r.size(), 2);
  std::cout << cppcommon::ToString(r) << std::endl;

  // case 7
  s = ".";
  cppcommon::StringSplit(r, s, delimeter);
  ASSERT_EQ(r.size(), 2);
  std::cout << cppcommon::ToString(r) << std::endl;
  cppcommon::StringSplit(r, s, delimeter, true);
  ASSERT_EQ(r.size(), 0);
  std::cout << cppcommon::ToString(r) << std::endl;
}

TEST(StrSplitWithFilter, Base) {
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

TEST(StringSplit, SV) {
  std::string s = R"(a:1,b:2,c:3)";
  std::vector<std::string_view> r;
  cppcommon::StringSplit(r, s, ',');
  std::cout << "StringSplit.SV [0]: " << cppcommon::ToString(r) << std::endl;
  ASSERT_EQ(r.size(), 3);
  ASSERT_EQ(r[0], "a:1");
  std::vector<std::string_view> r1r;
  cppcommon::StringSplit(r1r, r[0], ':');
  ASSERT_EQ(r1r.size(), 2);
  ASSERT_EQ(r1r[0], "a");
  ASSERT_EQ(r1r[1], "1");
  std::cout << "StringSplit.SV [1]: " << cppcommon::ToString(r1r) << std::endl;
}

TEST(Str, ToString) {
  std::unordered_map<std::string, int> si = {{"a", 1}, {"b", 2}};
  std::unordered_map<int, std::unordered_map<std::string, int>> im = {{101, si}};
  spdlog::info("si={}, im={}", cppcommon::ToString(si), cppcommon::ToString(im));

  auto s = R"("Beijing": "China")";
  spdlog::info("{}", s);

  std::unordered_map<std::string, std::string> ss = {{"a", "1"}, {"b", "2"}};
  std::vector<std::unordered_map<std::string, std::string>> ssv = {ss, ss};
  spdlog::info("{}", cppcommon::ToString(ss));
  spdlog::info("{}", cppcommon::ToString(ssv));

  std::vector<std::string> sv = {"aaaa", "bbbb"};
  spdlog::info("{}", cppcommon::ToString(sv));
  spdlog::info("{}", cppcommon::ToString(111));
  spdlog::info("{}", cppcommon::ToString(12));
  spdlog::info("{}", cppcommon::ToString("121312"));
  spdlog::info("{}", cppcommon::ToString("dddd"));
}

TEST(StrSplitWithFilter, Badcase) {
  std::string t = "106";
  std::vector<std::string_view> svs;
  cppcommon::StrSplitWithFilter(svs, t, ',', "-"sv, true);
  spdlog::info("svs: {}", cppcommon::ToString(svs));
  ASSERT_EQ(svs.size(), 1);
  ASSERT_EQ(svs[0], "106");

  std::vector<std::string> sv;
  cppcommon::StrSplitWithFilter(sv, t, ',', "-"sv, true);
  spdlog::info("svs: {}", cppcommon::ToString(sv));
  ASSERT_EQ(sv.size(), 1);
  ASSERT_EQ(sv[0], "106");
}
