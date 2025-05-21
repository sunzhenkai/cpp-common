/**
 * @file str.h
 * @brief string utils
 * @author zhenkai.sun
 * @date 2025-03-21 09:37:57
 */
#pragma once
// sys
#include <spdlog/spdlog.h>

#include <cstring>
#include <string>
#include <string_view>
#include <vector>

using std::string_view_literals::operator""sv;
namespace cppcommon {
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
        // result.emplace_back(data.substr(start, end - start));
        result.emplace_back(data.data() + start, end - start);
      }
    }
    start = end + 1;
    end = data.find(delimeter, start);
  }

  if (start == data.size() || std::strncmp(data.data() + start, filter.data(), data.size() - start) != 0) {
    if (!ignore_empty || data.size() > start) {
      // result.emplace_back(data.substr(start));
      result.emplace_back(data.data() + start, data.size() - start);
    }
  }
}

inline void ToUpper(char *s) {
  while ((*s) != '\0') {
    *s = static_cast<char>(std::toupper(static_cast<unsigned char>(*s)));
    ++s;
  }
}

template <typename S>
inline std::string ToUpper(const S &src) {
  std::string result(src.data(), src.size());
  for (char &ch : result) {
    ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
  }
  return result;
}

inline bool StartsWith(std::string_view str, std::string_view prefix) {
  return str.size() >= prefix.size() && std::equal(prefix.begin(), prefix.end(), str.begin());
}

inline bool EndsWith(std::string_view str, std::string_view suffix) {
  return str.size() >= suffix.size() && std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}
}  // namespace cppcommon
