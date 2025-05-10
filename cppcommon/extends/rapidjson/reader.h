
/**
 * @file reader.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-05-10 18:18:30
 */
#pragma once
#include <string>
#include <utility>

#include "cppcommon/utils/type_traits.h"
#include "rapidjson/document.h"

namespace cppcommon {
class JsonReader {
 public:
  explicit JsonReader(const std::string& json_str);
  bool HasError() const;

  template <typename T, typename... Args>
  T Get(const T& dft, Args&&... args) const {
    const rapidjson::Value* node = &doc_;
    return GetValueImpl<T>(node, dft, std::forward<Args>(args)...);
  }

 private:
  template <typename T, typename Arg, typename... Rest>
  T GetValueImpl(const rapidjson::Value* node, const T& dft, Arg&& arg, Rest&&... rest) const {
    if (!(node) || !(node)->IsObject() || !(node)->HasMember(arg)) {
      return dft;
    }
    node = &(node)->operator[](arg);
    return GetValueImpl<T>(node, dft, std::forward<Rest>(rest)...);
  }

  template <typename T, std::enable_if_t<!cppcommon::is_vector_v<T>, int> = 0>
  T GetValueImpl(const rapidjson::Value* node, const T& dft) const {
    if (!(node)) return dft;
    if constexpr (std::is_same_v<T, std::string>) {
      return (node)->IsString() ? (node)->GetString() : dft;
    } else if constexpr (std::is_integral_v<T>) {
      return (node)->IsInt() ? (node)->GetInt() : dft;
    } else if constexpr (std::is_floating_point_v<T>) {
      return (node)->IsDouble() ? (node)->GetDouble() : dft;
    } else if constexpr (std::is_same_v<T, bool>) {
      return (node)->IsBool() ? (node)->GetBool() : dft;
    }
    return dft;
  }

  template <typename T, std::enable_if_t<cppcommon::is_vector_v<T>, int> = 0>
  T GetValueImpl(const rapidjson::Value* node, const T& dft) const {
    if (!node || !node->IsArray()) {
      return dft;
    }
    T res;
    for (auto it = node->Begin(); it != node->End(); ++it) {
      res.emplace_back(GetValueImpl(it, VectorElementType_t<T>{}));
    }
    return res;
  }

  rapidjson::Document doc_;
};

inline JsonReader::JsonReader(const std::string& json_str) { doc_.Parse(json_str.c_str()); }

inline bool JsonReader::HasError() const { return doc_.HasParseError(); }
}  // namespace cppcommon
