/**
 * @file differ.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-06-09 10:25:09
 */
#pragma once

#include <absl/status/status.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <algorithm>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "cppcommon/extends/rapidjson/utils.h"
#include "cppcommon/utils/differ.h"

namespace cppcommon::rapidjson {
enum class DiffType { Less, More, TypeMismatch, Diff };

inline std::string DiffTypeToString(DiffType type) {
  switch (type) {
    case DiffType::Less:
      return "Less";
    case DiffType::More:
      return "More";
    case DiffType::TypeMismatch:
      return "TypeMismatch";
    case DiffType::Diff:
      return "Diff";
    default:
      return "Unknown";
  }
}

struct DiffItem {
  std::string path;
  DiffType type;
  std::string left_value;
  std::string right_value;
  ElementResult detail;

  std::string ToString(const std::string& prefix = "", bool with_path = true) const {
    std::ostringstream oss;
    if (with_path) {
      oss << prefix << path << ": ";
    }
    switch (type) {
      case DiffType::Less:
        oss << prefix << "-(" << right_value << ")";
        break;
      case DiffType::More:
        oss << prefix << "+(" << left_value << ")";
        break;
      case DiffType::TypeMismatch:
        oss << prefix << "T(" << left_value << ") != T(" << right_value << ")";
        break;
      case DiffType::Diff:
        oss << prefix << detail.DiffString();
        // oss << prefix << "(" << left_value << ") != (" << right_value << ")";
        break;
    }
    return oss.str();
  }
};

struct DiffResult {
  std::vector<DiffItem> items;

  inline std::string ToString(const std::string& prefix = "") const {
    std::stringstream ss;
    for (auto& item : items) {
      ss << prefix << item.ToString() << std::endl;
    }
    return ss.str();
  }
};

struct DiffValueStat {
  std::unordered_map<std::string, int> value_count;

  inline void Add(const std::string& v) { ++value_count[v]; }

  inline std::string ToString(const std::string& prefix = "") const {
    std::stringstream ss;
    for (auto& [k, v] : value_count) {
      ss << prefix << "(" << v << ") " << k << std::endl;
    }
    return ss.str();
  }
};

struct BatchDiffResultStat {
  size_t diff_count{0}, same_count{0};
  std::vector<DiffResult> records;

  std::map<std::string, std::vector<const DiffItem*>> path_stat;
  std::map<DiffType, std::vector<const DiffItem*>> type_stat;

  inline void AddDiffResult(DiffResult&& dr) {
    for (auto& item : dr.items) {
      path_stat[item.path].emplace_back(&item);
      type_stat[item.type].emplace_back(&item);
    }
    if (dr.items.empty()) {
      ++same_count;
    } else {
      ++diff_count;
    }
    records.emplace_back(std::move(dr));
  }

  inline size_t TotalCount() const { return records.size(); }

  inline std::string ToString() const {
    std::stringstream ss;
    // 1. summary
    ss << "--- Summary ---" << std::endl;
    ss << "total=" << TotalCount() << ", diff=" << diff_count << ", same=" << same_count << std::endl;
    if (!path_stat.empty()) {
      ss << "diff pathes: " << std::endl;
      for (auto& [k, v] : path_stat) {
        ss << "  " << k << ", count=" << v.size() << std::endl;
      }
    }
    if (!type_stat.empty()) {
      ss << "diff types: " << std::endl;
      for (auto& [k, v] : type_stat) {
        ss << "  " << DiffTypeToString(k) << ", count=" << v.size() << std::endl;
      }
    }
    // 2. by diff pathes
    if (!path_stat.empty()) {
      ss << "--- by diff pathes ---" << std::endl;
      for (auto& [k, v] : path_stat) {
        ss << "#" << k << std::endl;
        DiffValueStat stat;
        for (auto& item : v) {
          stat.Add(item->ToString("", false));
        }
        ss << stat.ToString("  ");
      }
    }
    // 3. by diff types
    if (!type_stat.empty()) {
      ss << "--- by diff types ---" << std::endl;
      for (auto& [k, v] : type_stat) {
        ss << "#" << DiffTypeToString(k) << std::endl;
        DiffValueStat stat;
        for (auto& item : v) {
          stat.Add(item->ToString());
        }
        ss << stat.ToString("  ");
      }
    }
    // 4. by records
    if (diff_count > 0) {
      ss << "--- by records ---" << std::endl;
      for (size_t i = 0; i < records.size(); ++i) {
        if (!records[i].items.empty()) {
          ss << "#" << i << ":" << std::endl;
          ss << records[i].ToString("  ");
        }
      }
    }
    return ss.str();
  }
};

inline void AddResultToDiffs(const CompareOptions& options, std::vector<DiffItem>& diffs, DiffItem&& item) {
  const std::string& path = item.path;

  for (const auto& pattern : options.ignore_pathes) {
    try {
      if (std::regex_match(path, std::regex(pattern))) {
        return;
      }
    } catch (const std::regex_error& e) {
      continue;
    }
  }

  if (!options.filter_pathes.empty()) {
    bool matched = false;
    for (const auto& pattern : options.filter_pathes) {
      try {
        if (std::regex_match(path, std::regex(pattern))) {
          matched = true;
          break;
        }
      } catch (const std::regex_error& e) {
        continue;
      }
    }
    if (!matched) return;
  }

  diffs.push_back(std::move(item));
}

inline void DiffJson(const CompareOptions& options, const ::rapidjson::Value& lhs, const ::rapidjson::Value& rhs,
                     const std::string& path, std::vector<DiffItem>& diffs) {
  if (lhs.GetType() != rhs.GetType()) {
    AddResultToDiffs(options, diffs, {path, DiffType::TypeMismatch, JsonValueToString(lhs), JsonValueToString(rhs)});
    return;
  }

  switch (lhs.GetType()) {
    case ::rapidjson::kObjectType: {
      for (auto itr = lhs.MemberBegin(); itr != lhs.MemberEnd(); ++itr) {
        std::string key = itr->name.GetString();
        if (rhs.HasMember(key.c_str())) {
          DiffJson(options, itr->value, rhs[key.c_str()], path + "/" + key, diffs);
        } else {
          AddResultToDiffs(options, diffs, {path + "/" + key, DiffType::More, JsonValueToString(itr->value), ""});
        }
      }
      for (auto itr = rhs.MemberBegin(); itr != rhs.MemberEnd(); ++itr) {
        std::string key = itr->name.GetString();
        if (!lhs.HasMember(key.c_str())) {
          AddResultToDiffs(options, diffs, {path + "/" + key, DiffType::Less, "", JsonValueToString(itr->value)});
        }
      }
      break;
    }
    case ::rapidjson::kArrayType: {
      size_t min_size = std::min(lhs.Size(), rhs.Size());
      for (size_t i = 0; i < min_size; ++i) {
        DiffJson(options, lhs[i], rhs[i], path + "[" + std::to_string(i) + "]", diffs);
      }
      for (size_t i = min_size; i < lhs.Size(); ++i) {
        AddResultToDiffs(options, diffs,
                         {path + "[" + std::to_string(i) + "]", DiffType::More, JsonValueToString(lhs[i]), ""});
      }
      for (size_t i = min_size; i < rhs.Size(); ++i) {
        AddResultToDiffs(options, diffs,
                         {path + "[" + std::to_string(i) + "]", DiffType::Less, "", JsonValueToString(rhs[i])});
      }
      break;
    }
    default: {
      auto r = cppcommon::CompareString(options, JsonValueToString(lhs), JsonValueToString(rhs));
      if (!r) {
        AddResultToDiffs(options, diffs,
                         {path, DiffType::Diff, JsonValueToString(lhs), JsonValueToString(rhs), std::move(r)});
      }
    }
  }
}

inline absl::Status DiffJson(BatchDiffResultStat& result, const CompareOptions& options, const std::string& lhs,
                             const std::string& rhs, bool allow_empty = false) {
  ::rapidjson::Document ldoc, rdoc;
  if (!ParseJson(lhs, ldoc, allow_empty).ok() || !ParseJson(rhs, rdoc, allow_empty).ok()) {
    return absl::InternalError("parse json string failed.");
  }
  DiffResult diffs;
  DiffJson(options, ldoc, rdoc, "", diffs.items);
  result.AddDiffResult(std::move(diffs));
  return absl::OkStatus();
}
}  // namespace cppcommon::rapidjson
