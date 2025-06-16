/**
 * @file differ.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-06-08 21:13:50
 */
#pragma once

#include <algorithm>
#include <functional>
#include <string>
#include <vector>

#include "cppcommon/utils/str.h"
#include "cppcommon/utils/to_str.h"
namespace cppcommon {
template <typename T>
using CmpFunc = std::function<bool(const T &, const T &)>;

struct CompareOptions {
  struct VectorOptions {
    char delimiter{'\0'};
    bool ignore_order{false};
  };

  VectorOptions vector_options;
  std::vector<std::string> filter_pathes;
  std::vector<std::string> ignore_pathes;
};

struct ElementResult {
  std::string key;
  std::vector<std::string> less;
  std::vector<std::string> more;
  std::vector<std::string> same;

  explicit operator bool() const { return less.empty() && more.empty(); }

  std::string DiffString() const {
    std::stringstream ss;
    if (!key.empty()) {
      ss << key << ": ";
    }
    if (same.size() == 0 && less.size() == 1 && more.size() == 1) {
      ss << less[0] << " != " << more[0];
    } else {
      for (auto &v : less) {
        ss << "-(" << v << ") ";
      }
      for (auto &v : more) {
        ss << "+(" << v << ") ";
      }
    }
    return ss.str();
  }
};

template <typename T>
bool DefaultCompare(const CompareOptions &options, const T &a, const T &b) {
  return a == b;
}

template <typename T>
ElementResult CompareVector(const CompareOptions &options, const std::vector<T> &a, const std::vector<T> &b) {
  auto av = a;
  auto bv = b;
  std::sort(av.begin(), av.end());
  std::sort(bv.begin(), bv.end());

  ElementResult result;
  size_t ia{0}, ib{0};

  auto comp = [&options](const auto &a, const auto &b) { return DefaultCompare(options, a, b); };

  while (ia < av.size() && ib < bv.size()) {
    if (comp(av[ia], bv[ib])) {
      result.same.emplace_back(cppcommon::ToNoQueoteString(av[ia]));
      ++ia;
      ++ib;
    } else if (av[ia] < bv[ib]) {
      result.less.emplace_back(cppcommon::ToNoQueoteString(av[ia]));
      ++ia;
    } else {
      result.more.emplace_back(cppcommon::ToNoQueoteString(bv[ib]));
      ++ib;
    }
  }

  while (ia < av.size()) {
    result.less.emplace_back(av[ia]);
    ++ia;
  }

  while (ib < bv.size()) {
    result.more.emplace_back(bv[ib]);
    ++ib;
  }

  return result;
}

template <typename T>
ElementResult CompareString(const CompareOptions &options, const T &a, const T &b) {
  std::vector<std::string> av, bv;
  cppcommon::StringSplit(av, a, options.vector_options.delimiter);
  cppcommon::StringSplit(bv, b, options.vector_options.delimiter);
  return CompareVector(options, av, bv);
}
}  // namespace cppcommon
