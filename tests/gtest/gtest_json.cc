#include <rapidjson/document.h>

#include <iostream>

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
  // std::cout << cppcommon::JsonBuilder::From("abc", 123) << std::endl;
  // std::cout << cppcommon::JsonBuilder::From("abc", "123") << std::endl;
}
