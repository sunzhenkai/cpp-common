/**
 * @file utils.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-06-09 10:34:39
 */
#pragma once
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <string>

#include "absl/status/status.h"

namespace cppcommon {

inline std::string JsonValueToString(const rapidjson::Value& val) {
  if (val.IsString()) {
    return val.GetString();
  }
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  val.Accept(writer);
  return buffer.GetString();
}

inline absl::Status ParseJson(const std::string& json_str, rapidjson::Document& doc, bool allow_empty = false) {
  if (allow_empty && json_str.empty()) {
    doc.SetObject();
    return absl::OkStatus();
  }
  doc.Parse(json_str.c_str());
  if (doc.HasParseError()) {
    return absl::InternalError(GetParseError_En(doc.GetParseError()));
  }
  return absl::OkStatus();
}

}  // namespace cppcommon
