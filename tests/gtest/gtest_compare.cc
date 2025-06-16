#include <spdlog/spdlog.h>

#include <string>

#include "cppcommon/extends/rapidjson/differ.h"
#include "cppcommon/utils/differ.h"
#include "gtest/gtest.h"

TEST(Cmp, V) {
  std::string a = "c,a,b";
  std::string b = "d,a,c";

  auto r = cppcommon::CompareString({.vector_options{.delimiter = ','}}, a, b);
  spdlog::info("{}", r.DiffString());
}

TEST(Cmp, JS) {
  // record 1
  std::string json1 = R"({"a": 1, "b": [1, 2], "c": {"d": "x"}})";
  std::string json2 = R"({"a": 2, "b": [1, 3], "c": {"d": "x"}, "e": 5})";
  cppcommon::rapidjson::BatchDiffResultStat stat;
  auto s = cppcommon::rapidjson::DiffJson(stat, {}, json1, json2);

  // record 2
  json1 = R"({"a": 1, "b": [1, 2, 3], "c": {"d": "x"}, "f": "1,2,3"})";
  json2 = R"({"a": 2, "b": [1, 3], "c": {"d": "x"}, "e": 5, "f": "1,3,5"})";
  s = cppcommon::rapidjson::DiffJson(stat, {.vector_options{.delimiter = ','}, .filter_pathes = {"a"}}, json1, json2);
  spdlog::info("{}: {}", s.ToString(), stat.ToString());
}

TEST(Cmp, JsV2) {
  cppcommon::rapidjson::BatchDiffResultStat stat;
  spdlog::info("{}", stat.ToString());
}

TEST(Cmp, JsV3) {
  std::string json1 = R"({"a": 1, "b": [1, 2, 3], "c": {"d": "x"}, "f": "1,2,3"})";
  std::string json2 = R"({"a": 2, "b": [1, 3], "c": {"d": "x"}, "e": 5, "f": "1,3,5"})";
  cppcommon::rapidjson::BatchDiffResultStat stat;
  auto s = cppcommon::rapidjson::DiffJson(stat, {.vector_options{.delimiter = ','}, .filter_pathes = {".*a", "/b.*"}},
                                          json1, json2);
  spdlog::info("{}: {}", s.ToString(), stat.ToString());
}
