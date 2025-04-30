#include <rapidjson/document.h>

#include <iostream>
#include <string>

#include "cppcommon/extends/rapidjson/builder.h"
#include "gtest/gtest.h"

TEST(Json, From) {
  rapidjson::Document doc;
  auto &alc = doc.GetAllocator();
  cppcommon::ToValue("123", alc);
  cppcommon::ToValue(123, alc);
  cppcommon::ToValue(true, alc);
  cppcommon::ToValue(1.1, alc);
  cppcommon::ToValue(std::vector<int>{1, 2, 3}, alc);
  cppcommon::ToValue(std::array<int, 3>{1, 2, 3}, alc);
  cppcommon::ToValue(std::unordered_map<int, int>{}, alc);

  std::string s;

  // case
  s = cppcommon::JsonBuilder::From("abc", 123);
  std::cout << s << std::endl;
  ASSERT_EQ(s, R"({"abc":123})");

  // case
  s = cppcommon::JsonBuilder::From("abc", "123");
  std::cout << s << std::endl;
  ASSERT_EQ(s, R"({"abc":"123"})");

  // case
  s = cppcommon::JsonBuilder::From("abc", 3.14);
  std::cout << s << std::endl;
  ASSERT_EQ(s, R"({"abc":3.14})");

  // case
  s = cppcommon::JsonBuilder::From("abc", true);
  std::cout << s << std::endl;
  ASSERT_EQ(s, R"({"abc":true})");

  // case
  s = cppcommon::JsonBuilder::From(std::vector<int>{1, 2, 3});
  std::cout << s << std::endl;
  ASSERT_EQ(s, R"([1,2,3])");

  // case
  s = cppcommon::JsonBuilder::From(std::unordered_map<int, int>{{1, 1}, {2, 2}});
  std::cout << s << std::endl;
  ASSERT_EQ(s, R"({"1":1,"2":2})");
}
