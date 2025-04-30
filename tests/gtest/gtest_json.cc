#include <rapidjson/document.h>

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

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
  s = cppcommon::JsonBuilder::From(std::unordered_map<int, int>{{1, 1}});
  std::cout << s << std::endl;
  ASSERT_EQ(s, R"({"1":1})");
}

TEST(Json, Builder) {
  cppcommon::JsonBuilder jb;
  jb.Add("a", "1");
  jb.Add("b", 1);
  jb.Add("cc", std::vector<int>{1, 2, 3});
  jb.Add("dd", std::vector<int>{1, 2, 3});
  jb.Add("eee", std::map<std::string, int>{{"1", 1}});
  jb.Add("fff", std::set<int>{1, 3, 5});
  std::cout << jb.Build() << std::endl;
  ASSERT_EQ(jb.Build(), R"({"a":"1","b":1,"cc":[1,2,3],"dd":[1,2,3],"eee":{"1":1},"fff":[1,3,5]})");

  cppcommon::JsonBuilder jb2;
  jb2.Add("hello", "neo");
  jb2.Add("other", jb);
  jb2.AddJsonStr("foo", R"({"foo":"bar"})");

  std::cout << jb2.Build() << std::endl;
  ASSERT_EQ(
      jb2.Build(),
      R"({"hello":"neo","other":{"a":"1","b":1,"cc":[1,2,3],"dd":[1,2,3],"eee":{"1":1},"fff":[1,3,5]},"foo":{"foo":"bar"}})");
}
