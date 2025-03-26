/**
 * @file str.h
 * @brief string utils
 * @author zhenkai.sun
 * @date 2025-03-21 09:37:57
 */
#pragma once
// sys
#include <cstring>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

using namespace std::string_view_literals;
namespace cppcommon {
inline std::string ToString(int argc, char **argv) {
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
std::string ToString(const std::vector<T> &v) {
  std::stringstream ss;
  ss << "[";
  for (size_t i = 0; i < v.size(); ++i) {
    if (i != 0) ss << ",";
    ss << "\"" << v[i] << "\"";
  }
  ss << "]";
  return ss.str();
}

template <typename T>
std::string ToString(const T &v) {
  std::stringstream ss;
  ss << '"' << v << '"';
  return ss.str();
}

template <typename K, typename V>
std::string ToString(const std::unordered_map<K, V> &m) {
  std::stringstream ss;
  ss << "{";
  for (auto it = m.begin(); it != m.end(); ++it) {
    if (it != m.begin()) ss << ",";
    ss << ToString(it->first) << ":" << ToString(it->second);
  }
  ss << "}";
  return ss.str();
}

template <typename R, typename D>
void StringSplit(std::vector<R> &result, const D &data, const char &delimeter, bool ignore_empty = false) {
  result.clear();
  size_t start = 0;
  size_t end = data.find(delimeter);
  while (end != std::string::npos) {
    if (!ignore_empty || end > start) {
      result.emplace_back(data.data() + start, end - start);
    }
    start = end + 1;
    end = data.find(delimeter, start);
  }
  if (!ignore_empty || data.size() > start) {
    result.emplace_back(data.data() + start, data.size() - start);
  }
}

template <typename R, typename D, typename F>
void StrSplitWithFilter(std::vector<R> &result, const D &data, const char &delimeter, const F filter,
                        bool ignore_empty = false) {
  result.clear();
  size_t start = 0;
  size_t end = data.find(delimeter);
  while (end != std::string::npos) {
    if (end == start || std::strncmp(data.data() + start, filter.data(), end - start) != 0) {
      if (!ignore_empty || end > start) {
        result.emplace_back(data.substr(start, end - start));
      }
    }
    start = end + 1;
    end = data.find(delimeter, start);
  }

  if (start == data.size() || std::strncmp(data.data() + start, filter.data(), data.size() - start) != 0) {
    if (!ignore_empty || data.size() > start) {
      result.emplace_back(data.substr(start));
    }
  }
}
}  // namespace cppcommon
