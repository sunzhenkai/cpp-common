/**
 * @file str.h
 * @brief string utils
 * @author zhenkai.sun
 * @date 2025-03-21 09:37:57
 */
#pragma once
// sys
#include <spdlog/spdlog.h>

#include <cctype>
#include <cstring>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "cppcommon/utils/type_traits.h"

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

inline void ToLower(char *s) {
  while ((*s) != '\0') {
    *s = static_cast<char>(std::tolower(static_cast<unsigned char>(*s)));
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

template <typename S>
inline std::string ToLower(const S &src) {
  std::string result(src.data(), src.size());
  for (char &ch : result) {
    ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
  }
  return result;
}

inline bool StartsWith(std::string_view str, std::string_view prefix) {
  return str.size() >= prefix.size() && std::equal(prefix.begin(), prefix.end(), str.begin());
}

inline bool EndsWith(std::string_view str, std::string_view suffix) {
  return str.size() >= suffix.size() && std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

template <typename T>
std::enable_if_t<is_string_like_v<T> || is_string_literal_v<T>, bool> IsEmpty(T &val) {
  using U = unwrap_type_t<T>;
  if constexpr (std::is_same_v<U, std::string> || std::is_same_v<U, std::string_view>) {
    return val.empty();
  } else if constexpr (std::is_same_v<U, char *> || std::is_same_v<U, const char *>) {
    return val == nullptr || *val == '\0';
  } else if constexpr (cppcommon::is_string_literal_v<U>) {
    return val[0] == '\0';
  } else {
    static_assert(always_false<T>::value, "Unsupported type");
  }
}

inline bool IsSpace(char c) { return std::isspace(c); }

// https://en.wikipedia.org/wiki/Whitespace_character#Unicode
inline bool IsUnicodeSpace(char32_t code_point) {
  switch (code_point) {
    case 0x0009:
    case 0x000A:
    case 0x000B:
    case 0x000C:
    case 0x000D:
    case 0x0020:
    case 0x0085:
    case 0x00A0:
    case 0x1680:
    case 0x2000:
    case 0x2001:
    case 0x2002:
    case 0x2003:
    case 0x2004:
    case 0x2005:
    case 0x2006:
    case 0x2007:
    case 0x2008:
    case 0x2009:
    case 0x200A:
    case 0x2028:
    case 0x2029:
    case 0x202F:
    case 0x205F:
    case 0x3000:
      return true;
    default:
      return false;
  }
}

template <typename S>
inline std::string Trim(const S &s, const std::function<bool(char)> &is_space = IsSpace) {
  if (IsEmpty(s)) return std::string(s);
  using U = unwrap_type_t<S>;
  if constexpr (std::is_same_v<U, std::string> || std::is_same_v<U, std::string_view>) {
    size_t l = 0;
    size_t r = s.size() - 1;
    while (l <= r && is_space(s[l])) ++l;
    while (l <= r && is_space(s[r])) --r;
    if (l > r) return "";
    return std::string(s.data() + l, r - l + 1);
  } else if constexpr (std::is_same_v<U, char *> || std::is_same_v<U, const char *>) {
    return Trim(std::string_view(s), is_space);
  } else if constexpr (cppcommon::is_string_literal_v<U>) {
    return Trim(std::string_view(reinterpret_cast<const char *>(s)), is_space);
  } else {
    static_assert(always_false<S>::value, "Unsupported type");
  }
}
}  // namespace cppcommon
