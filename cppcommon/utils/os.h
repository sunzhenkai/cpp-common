/**
 * @file os.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-03-19 15:09:01
 */
#pragma once
#include <filesystem>
#include <sstream>
#include <string>

#include "cppcommon/utils/time.h"

namespace cppcommon {
/**
 * @brief Get envrionment variable with default value.
 * @param [in] key envrionment variable key
 * @param [in] dft default value
 */
std::string GetEnv(const std::string &key, const std::string &dft);

inline std::string GetDatedFilePath(int64_t ts_ms, const std::string &filename, const std::string &prefix,
                                    int timezone_offset = 0, bool year = true, bool month = true, bool day = true,
                                    bool hour = true) {
  auto di = GetDateInfo(ts_ms, timezone_offset);
  std::stringstream ss;
  ss << prefix << "/";
  if (year) {
    ss << di.GetHumanYear() << "/";
  }
  if (month) {
    auto v = di.GetHumanMonth();
    if (v < 10) ss << "0";
    ss << v << "/";
  }
  if (day) {
    auto v = di.GetMonthDay();
    if (v < 10) ss << "0";
    ss << v << "/";
  }
  if (hour) {
    auto v = di.GetHour();
    if (v < 10) ss << "0";
    ss << v << "/";
  }
  ss << filename;
  return ss.str();
}

inline std::string GetDatedFilePath(const std::string &filename, const std::string &prefix, int timezone_offset = 0,
                                    bool year = true, bool month = true, bool day = true, bool hour = true) {
  return GetDatedFilePath(CurrentTsMs(), filename, prefix, timezone_offset, year, month, day, hour);
}

template <typename S>
inline std::string GetFileName(const S &file_path) {
  return std::filesystem::path(file_path).filename().string();
}
}  // namespace cppcommon
