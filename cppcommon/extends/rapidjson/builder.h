/**
 * @file builder.h
 * @brief rapidjson builder
 * @author zhenkai.sun
 * @date 2025-04-08 14:44:21
 */
#pragma once
#include <string>
#include <type_traits>
#include <vector>

#include "cppcommon/utils/to_str.h"
#include "cppcommon/utils/type_traits.h"
#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/writer.h"

namespace cppcommon {
class JsonBuilder {
 public:
  JsonBuilder();

  // for sequence
  template <typename V, std::enable_if_t<cppcommon::is_container_v<V> && !cppcommon::is_map_v<V>, int> = 0>
  static std::string From(const V &v);

  // for k-v containers
  template <typename V, std::enable_if_t<cppcommon::is_map_v<V>, int> = 0>
  static std::string From(const V &v);

  template <typename K, typename V>
  static std::string From(const K &k, const V &v);

 private:
  rapidjson::Document doc_;
};

inline std::string ToJsonString(rapidjson::Document &doc) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);
  return buffer.GetString();
}

inline std::string ToJsonString(const rapidjson::Value &value) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  value.Accept(writer);
  return buffer.GetString();
}

template <typename V, std::enable_if_t<std::is_arithmetic_v<V> || cppcommon::is_string_literal_v<V>, int> = 0>
rapidjson::Value ToValue(const V &v, rapidjson::Document::AllocatorType &alc) {
  return rapidjson::Value(v);
}

template <typename V, std::enable_if_t<cppcommon::is_string_like_v<V>, int> = 0>
rapidjson::Value ToValue(const V &v, rapidjson::Document::AllocatorType &alc) {
  return rapidjson::Value(v.c_str(), v.size(), alc);
}

template <typename V, std::enable_if_t<cppcommon::is_container_v<V> && !cppcommon::is_map_v<V>, int> = 0>
rapidjson::Value ToValue(const V &v, rapidjson::Document::AllocatorType &alc) {
  rapidjson::Value ret;
  ret.SetArray();
  for (auto it = v.begin(); it != v.end(); ++it) {
    ret.PushBack(ToValue(*it, alc), alc);
  }
  return ret;
}

template <typename V, std::enable_if_t<cppcommon::is_map_v<V>, int> = 0>
rapidjson::Value ToValue(const V &val, rapidjson::Document::AllocatorType &alc) {
  rapidjson::Value ret;
  ret.SetObject();
  for (auto &[k, v] : val) {
    if constexpr (std::is_arithmetic_v<decltype(k)>) {
      ret.AddMember(ToValue(std::to_string(k), alc), ToValue(v, alc), alc);
    } else {
      ret.AddMember(ToValue(k, alc), ToValue(v, alc), alc);
    }
  }
  return ret;
}

// for sequence
template <typename V, std::enable_if_t<cppcommon::is_container_v<V> && !cppcommon::is_map_v<V>, int>>
std::string JsonBuilder::From(const V &v) {
  rapidjson::Document doc;
  doc.SetArray();
  auto &alc = doc.GetAllocator();
  for (auto it = v.begin(); it != v.end(); ++it) {
    doc.PushBack(ToValue(*it, alc), alc);
  }
  return ToJsonString(doc);
}

// for k-v containers
template <typename V, std::enable_if_t<cppcommon::is_map_v<V>, int>>
std::string JsonBuilder::From(const V &val) {
  rapidjson::Document doc;
  doc.SetObject();
  auto &alc = doc.GetAllocator();
  for (auto &[k, v] : val) {
    if constexpr (std::is_arithmetic_v<decltype(k)>) {
      doc.AddMember(ToValue(std::to_string(k), alc), ToValue(v, alc), alc);
    } else {
      doc.AddMember(ToValue(k, alc), ToValue(v, alc), alc);
    }
  }
  return ToJsonString(doc);
}

template <typename K, typename V>
std::string JsonBuilder::From(const K &k, const V &v) {
  rapidjson::Document doc;
  doc.SetObject();
  auto &alc = doc.GetAllocator();
  if constexpr (std::is_arithmetic_v<decltype(k)>) {
    doc.AddMember(ToValue(std::to_string(k), alc), ToValue(v, alc), alc);
  } else {
    doc.AddMember(ToValue(k, alc), ToValue(v, alc), alc);
  }
  return ToJsonString(doc);
}

}  // namespace cppcommon
