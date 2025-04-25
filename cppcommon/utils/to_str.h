/**
 * @file to_str.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-04-25 16:12:21
 */
#pragma once
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>

namespace cppcommon {
inline std::string ToString(int argc, char** argv) {
  std::stringstream ss;
  ss << "[";
  for (int i = 0; i < argc; ++i) {
    if (i != 0) ss << ",";
    ss << "\"" << *(argv + i) << "\"";
  }
  ss << "]";
  return ss.str();
}

template <typename T>
struct unwrap_type {
  using type = std::remove_cv_t<std::remove_reference_t<T>>;
};

template <typename T>
using unwrap_type_t = typename unwrap_type<T>::type;

template <typename T>
struct is_basic_or_string_like
    : std::disjunction<std::is_arithmetic<unwrap_type_t<T>>, std::is_same<unwrap_type_t<T>, std::string>,
                       std::is_same<unwrap_type_t<T>, std::string_view>> {};

template <typename T>
constexpr bool is_basic_or_string_like_v = is_basic_or_string_like<T>::value;

template <typename T>
struct is_string_like
    : std::disjunction<std::is_same<unwrap_type_t<T>, std::string>, std::is_same<unwrap_type_t<T>, std::string_view>> {
};

template <typename T>
constexpr bool is_string_like_v = is_string_like<T>::value;

template <typename T>
std::string ValueToString(const T& value) {
  std::stringstream ss;
  if constexpr (is_string_like_v<std::decay_t<T>>) {
    ss << std::quoted(value);
  } else if constexpr (std::is_floating_point_v<T>) {
    ss << std::fixed << std::setprecision(15) << value;
  } else if constexpr (std::is_same_v<std::decay_t<T>, bool>) {
    ss << (value ? "true" : "false");
  } else {
    ss << value;
  }
  return ss.str();
}

// Forward declaration for recursive calls
template <typename T>
std::string ContainerToString(const T& container);

// Specialization for map-like containers
template <typename Key, typename Value, typename Compare, typename Alloc>
std::string ContainerToString(const std::map<Key, Value, Compare, Alloc>& container) {
  std::stringstream ss;
  ss << "{";
  bool first = true;
  for (const auto& pair : container) {
    if (!first) {
      ss << ",";
    }
    ss << ValueToString(pair.first) << ":" << ContainerToString(pair.second);
    first = false;
  }
  ss << "}";
  return ss.str();
}

template <typename Key, typename Value, typename Hash, typename Pred, typename Alloc>
std::string ContainerToString(const std::unordered_map<Key, Value, Hash, Pred, Alloc>& container) {
  std::stringstream ss;
  ss << "{";
  bool first = true;
  for (const auto& pair : container) {
    if (!first) {
      ss << ",";
    }
    ss << ValueToString(pair.first) << ":" << ContainerToString(pair.second);
    first = false;
  }
  ss << "}";
  return ss.str();
}

// General case for other container types (assumed to be array-like)
template <template <typename...> class Container, typename T, typename... Args>
std::string ContainerToString(const Container<T, Args...>& container) {
  std::stringstream ss;
  ss << "[";
  bool first = true;
  for (const auto& item : container) {
    if (!first) {
      ss << ",";
    }
    if constexpr (is_basic_or_string_like_v<std::decay_t<T>>) {
      ss << ValueToString(item);
    } else {
      ss << ContainerToString(item);
    }
    first = false;
  }
  ss << "]";
  return ss.str();
}

// Handle basic types and strings
template <typename T>
std::string ContainerToString(const T& value) {
  return ValueToString(value);
}

// Handle std::variant
template <typename... Args>
std::string ContainerToString(const std::variant<Args...>& var) {
  return std::visit([](const auto& val) { return ContainerToString(val); }, var);
}

template <typename T>
std::string ToString(const T& v) {
  if constexpr (is_basic_or_string_like_v<T>) {
    return ValueToString(v);
  } else {
    return ContainerToString(v);
  }
}
}  // namespace cppcommon
