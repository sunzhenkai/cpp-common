/**
 * @file builder.h
 * @brief rapidjson builder
 * @author zhenkai.sun
 * @date 2025-04-08 14:44:21
 */
#pragma once
#include <string>
#include <type_traits>

#include "cppcommon/utils/type_traits.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"

namespace cppcommon {
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

class JsonBuilder {
 public:
  JsonBuilder();

  template <typename K, typename V>
  void Add(const K &k, const V &v);

  template <typename K>
  void Add(const K &k, const JsonBuilder &v);

  template <typename K, typename S>
  void AddJsonStr(const K &k, const S &js);

  inline std::string Build() const { return ToJsonString(doc_); }

  inline const rapidjson::Document &Doc() const { return doc_; }

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

template <typename V, std::enable_if_t<std::is_arithmetic_v<V> || cppcommon::is_string_literal_v<V>, int> = 0>
rapidjson::Value ToValue(const V &v, rapidjson::Document::AllocatorType &alc) {
  return rapidjson::Value(v);
}

template <typename V, std::enable_if_t<cppcommon::is_string_like_v<V>, int> = 0>
rapidjson::Value ToValue(const V &v, rapidjson::Document::AllocatorType &alc) {
  return rapidjson::Value(v.data(), v.size(), alc);
}

template <typename V>
rapidjson::Value ToKeyValue(const V &k, rapidjson::Document::AllocatorType &alc) {
  if constexpr (std::is_arithmetic_v<V>) {
    return ToValue(std::to_string(k), alc);
  } else {
    return ToValue(k, alc);
  }
}

template <typename V,
          std::enable_if_t<cppcommon::is_container_v<V> && !cppcommon::is_map_v<V> && !cppcommon::is_string_like_v<V>,
                           int> = 0>
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
    ret.AddMember(ToKeyValue(k, alc), ToValue(v, alc), alc);
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
    doc.AddMember(ToKeyValue(k, alc), ToValue(v, alc), alc);
  }
  return ToJsonString(doc);
}

template <typename K, typename V>
std::string JsonBuilder::From(const K &k, const V &v) {
  rapidjson::Document doc;
  doc.SetObject();
  auto &alc = doc.GetAllocator();
  doc.AddMember(ToKeyValue(k, alc), ToValue(v, alc), alc);
  return ToJsonString(doc);
}

template <typename K, typename V>
void JsonBuilder::Add(const K &k, const V &v) {
  auto &alc = doc_.GetAllocator();
  doc_.AddMember(ToKeyValue(k, alc), ToValue(v, alc), alc);
}

template <typename K>
void JsonBuilder::Add(const K &k, const JsonBuilder &v) {
  auto &alc = doc_.GetAllocator();
  rapidjson::Value nv(v.Doc(), alc);
  doc_.AddMember(ToKeyValue(k, alc), nv, alc);
}

template <typename K, typename S>
void JsonBuilder::AddJsonStr(const K &k, const S &js) {
  rapidjson::Document nd;
  if constexpr (std::is_same_v<std::string, unwrap_type_t<S>> || std::is_same_v<std::string_view, unwrap_type_t<S>>) {
    nd.Parse(js.data(), js.size());
  } else {
    nd.Parse(js);
  }

  auto &alc = doc_.GetAllocator();
  rapidjson::Value nv(nd, alc);
  doc_.AddMember(ToKeyValue(k, alc), nv, alc);
}
}  // namespace cppcommon
