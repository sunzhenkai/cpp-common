/**
 * @file differ.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-06-09 10:25:09
 */
#pragma once

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <algorithm>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "absl/status/status.h"
#include "cppcommon/extends/rapidjson/utils.h"

namespace cppcommon {
enum class DiffType { Less, More, TypeMismatch, ValueDiff };

struct DiffItem {
  std::string path;
  DiffType type;
  std::string left_value;
  std::string right_value;

  std::string ToString() const {
    std::ostringstream oss;
    oss << path << ": ";
    switch (type) {
      case DiffType::Less:
        oss << "-(" << right_value << ")";
        break;
      case DiffType::More:
        oss << "+(" << left_value << ")";
        break;
      case DiffType::TypeMismatch:
        oss << "T(" << left_value << ") != T(" << right_value << ")";
        break;
      case DiffType::ValueDiff:
        oss << "(" << left_value << ") != (" << right_value << ")";
        break;
    }
    return oss.str();
  }
};

struct DiffResult {
  std::vector<DiffItem> items;
};

struct BatchDiffResultStat {
  size_t diff_count{0}, same_count{0};
  std::vector<DiffResult> records;

  std::unordered_map<std::string, std::vector<const DiffItem*>> path_stat;
  std::unordered_map<DiffType, std::vector<const DiffItem*>> type_stat;

  inline void AddDiffResult(DiffResult&& dr) {
    for (auto& item : dr.items) {
      path_stat[item.path].emplace_back(&item);
      type_stat[item.type].emplace_back(&item);
    }
    records.emplace_back(std::move(dr));
  }

  inline size_t TotalCount() const { return records.size(); }
};

inline void DiffJson(const rapidjson::Value& lhs, const rapidjson::Value& rhs, const std::string& path,
                     std::vector<DiffItem>& diffs) {
  if (lhs.GetType() != rhs.GetType()) {
    diffs.push_back({path, DiffType::TypeMismatch, JsonValueToString(lhs), JsonValueToString(rhs)});
    return;
  }

  switch (lhs.GetType()) {
    case rapidjson::kObjectType: {
      for (auto itr = lhs.MemberBegin(); itr != lhs.MemberEnd(); ++itr) {
        std::string key = itr->name.GetString();
        if (rhs.HasMember(key.c_str())) {
          DiffJson(itr->value, rhs[key.c_str()], path + "/" + key, diffs);
        } else {
          diffs.push_back({path + "/" + key, DiffType::More, JsonValueToString(itr->value), ""});
        }
      }
      for (auto itr = rhs.MemberBegin(); itr != rhs.MemberEnd(); ++itr) {
        std::string key = itr->name.GetString();
        if (!lhs.HasMember(key.c_str())) {
          diffs.push_back({path + "/" + key, DiffType::Less, "", JsonValueToString(itr->value)});
        }
      }
      break;
    }
    case rapidjson::kArrayType: {
      size_t min_size = std::min(lhs.Size(), rhs.Size());
      for (size_t i = 0; i < min_size; ++i) {
        DiffJson(lhs[i], rhs[i], path + "[" + std::to_string(i) + "]", diffs);
      }
      for (size_t i = min_size; i < lhs.Size(); ++i) {
        diffs.push_back({path + "[" + std::to_string(i) + "]", DiffType::More, JsonValueToString(lhs[i]), ""});
      }
      for (size_t i = min_size; i < rhs.Size(); ++i) {
        diffs.push_back({path + "[" + std::to_string(i) + "]", DiffType::Less, "", JsonValueToString(rhs[i])});
      }
      break;
    }
    default: {
      if (JsonValueToString(lhs) != JsonValueToString(rhs)) {
        diffs.push_back({path, DiffType::ValueDiff, JsonValueToString(lhs), JsonValueToString(rhs)});
      }
    }
  }
}
}  // namespace cppcommon
