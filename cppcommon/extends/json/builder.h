/**
 * @file builder.h
 * @brief rapidjson builder
 * @author zhenkai.sun
 * @date 2025-04-08 14:44:21
 */
#pragma once
#include <string>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"

namespace cppcommon {
class JsonBuilder {
 public:
  JsonBuilder();

  template <typename V>
  std::string From(const V &v);

 private:
  rapidjson::Document doc_;
};

template <typename V>
std::string JsonBuilder::From(const V &v) {
  // doc_.ParseStream
  // return doc_;
  return "";
}
}  // namespace cppcommon
