#include <rapidjson/document.h>
#include <spdlog/spdlog.h>

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "cppcommon/extends/rapidjson/builder.h"
#include "cppcommon/extends/rapidjson/reader.h"
#include "cppcommon/utils/to_str.h"
#include "gtest/gtest.h"

using namespace std::string_view_literals;

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
  std::string_view pisv{"3.14"};
  jb2.Add("pi", pisv);
  jb2.AddJsonStr("foo", R"({"foo":"bar"})");

  std::cout << jb2.Build() << std::endl;
  ASSERT_EQ(
      jb2.Build(),
      R"({"hello":"neo","other":{"a":"1","b":1,"cc":[1,2,3],"dd":[1,2,3],"eee":{"1":1},"fff":[1,3,5]},"foo":{"foo":"bar"}})");

  std::string js = R"({"foo":"bar"})";
  jb2.AddJsonStr("foo", js.c_str());
  jb2.AddJsonStr("foo", js);
  jb2.AddJsonStr("foobar", "{}"sv);
}

TEST(Json, Reader) {
  auto s =
      R"({"hello":"neo","other":{"a":"1","b":1,"cc":[1,2,3],"dd":["1","2","3"],"eee":{"1":1},"fff":[1,3,5]},"foo":{"foo":"bar"}})";

  cppcommon::JsonReader jr(s);
  spdlog::info("HasError: {}", jr.HasError());
  ASSERT_TRUE(!jr.HasError());
  spdlog::info("hello: {}", jr.Get<std::string>("default", "hello"));
  ASSERT_EQ(jr.Get<std::string>("default", "hello"), "neo");
  ASSERT_EQ(jr.Get<std::string>("default", "hello_no"), "default");
  ASSERT_EQ(jr.Get<std::string>("default", "other", "a"), "1");
  ASSERT_EQ(jr.Get<int>(1, "hello", "other", "b"), 1);
  auto v = jr.Get<std::decay_t<std::vector<int>>>({}, "other", "cc");
  spdlog::info("{}", cppcommon::ToString(v));
  ASSERT_EQ(v.size(), 3);
  ASSERT_EQ(v[2], 3);

  auto vs = jr.Get<std::decay_t<std::vector<std::string>>>({}, "other", "dd");
  spdlog::info("{}", cppcommon::ToString(vs));
  ASSERT_EQ(vs.size(), 3);
  ASSERT_EQ(vs[2], "3");
}
