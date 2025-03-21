/**
 * @file str.h
 * @brief string utils
 * @author zhenkai.sun
 * @date 2025-03-21 09:37:57
 */
#pragma once
// sys
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace cppcommon {
template <typename T>
std::string ToString(const std::vector<T> &v) {
  std::stringstream ss;
  ss << "[";
  for (int i = 0; i < v.size(); ++i) {
    if (i != 0) ss << ",";
    ss << "\"" << v[i] << "\"";
  }
  ss << "]";
  return ss.str();
}

template <typename K, typename V>
std::string ToString(const std::unordered_map<K, V> &m) {
  std::stringstream ss;
  ss << "{";
  for (auto it = m.begin(); it != m.end(); ++it) {
    if (it != m.begin()) ss << ",";
    ss << "\"" << it->first << "\":" << "\"" << it->second << "\"";
  }
  ss << "}";
  return ss.str();
}

void StringSplit(std::vector<std::string> &result, const std::string &data, const char delimeter,
                 bool ignore_empty = false);
}  // namespace cppcommon
