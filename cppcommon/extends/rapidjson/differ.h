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
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "cppcommon/extends/rapidjson/utils.h"
#include "cppcommon/utils/differ.h"

namespace cppcommon::rapidjson {
enum class DiffType { Less, More, TypeMismatch, ValueDiff };

inline std::string DiffTypeToString(DiffType type) {
  switch (type) {
    case DiffType::Less:
      return "Less";
    case DiffType::More:
      return "More";
    case DiffType::TypeMismatch:
      return "TypeMismatch";
    case DiffType::ValueDiff:
      return "ValueDiff";
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

  std::string ToString(const std::string& prefix = "") const {
    std::ostringstream oss;
    oss << prefix << path << ": ";
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
      case DiffType::ValueDiff:
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
    ss << std::endl;
    ss << "--- Summary ---" << std::endl;
    ss << "total=" << TotalCount() << ", diff=" << diff_count << ", same=" << same_count << std::endl;
    ss << "diff pathes: " << std::endl;
    for (auto& [k, v] : path_stat) {
      ss << "  " << k << ", count=" << v.size() << std::endl;
    }
    ss << "diff types: " << std::endl;
    for (auto& [k, v] : type_stat) {
      ss << "  " << DiffTypeToString(k) << ", count=" << v.size() << std::endl;
    }
    ss << "--- by diff pathes ---" << std::endl;
    for (auto& [k, v] : path_stat) {
      ss << "#" << k << std::endl;
      for (auto& item : v) {
        ss << item->ToString("  ");
      }
      ss << std::endl;
    }
    ss << "--- by diff types ---" << std::endl;
    for (auto& [k, v] : type_stat) {
      ss << "  " << DiffTypeToString(k) << ", count=" << v.size() << std::endl;
      ss << "#" << DiffTypeToString(k) << std::endl;
      for (auto& item : v) {
        ss << item->ToString("  ");
      }
      ss << std::endl;
    }
    ss << "--- by records ---" << std::endl;
    for (size_t i = 0; i < records.size(); ++i) {
      ss << "#" << i << ":" << std::endl;
      ss << records[i].ToString("  ");
    }

    ss << std::endl;
    return ss.str();
  }
};

inline void DiffJson(const CompareOptions& options, const ::rapidjson::Value& lhs, const ::rapidjson::Value& rhs,
                     const std::string& path, std::vector<DiffItem>& diffs) {
  if (lhs.GetType() != rhs.GetType()) {
    diffs.push_back({path, DiffType::TypeMismatch, JsonValueToString(lhs), JsonValueToString(rhs)});
    return;
  }

  switch (lhs.GetType()) {
    case ::rapidjson::kObjectType: {
      for (auto itr = lhs.MemberBegin(); itr != lhs.MemberEnd(); ++itr) {
        std::string key = itr->name.GetString();
        if (rhs.HasMember(key.c_str())) {
          DiffJson(options, itr->value, rhs[key.c_str()], path + "/" + key, diffs);
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
    case ::rapidjson::kArrayType: {
      size_t min_size = std::min(lhs.Size(), rhs.Size());
      for (size_t i = 0; i < min_size; ++i) {
        DiffJson(options, lhs[i], rhs[i], path + "[" + std::to_string(i) + "]", diffs);
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
      auto r = cppcommon::CompareString(options, JsonValueToString(lhs), JsonValueToString(rhs));
      if (!r) {
        diffs.push_back({path, DiffType::ValueDiff, JsonValueToString(lhs), JsonValueToString(rhs), std::move(r)});
      }
    }
  }
}

inline absl::Status DiffJson(BatchDiffResultStat& result, const CompareOptions& options, const std::string& lhs,
                             const std::string& rhs) {
  ::rapidjson::Document ldoc, rdoc;
  if (!ParseJson(lhs, ldoc).ok() || !ParseJson(rhs, rdoc).ok()) {
    return absl::InternalError("parse json string failed.");
  }
  DiffResult diffs;
  DiffJson(options, ldoc, rdoc, "", diffs.items);
  result.AddDiffResult(std::move(diffs));
  return absl::OkStatus();
}
}  // namespace cppcommon::rapidjson
