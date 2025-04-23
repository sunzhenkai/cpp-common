/**
 * @file common.c
 * @brief
 * @author zhenkai.sun
 * @date 2025-03-25 21:50:05
 */
#pragma once
#include <chrono>
#include <exception>
#include <string>

namespace cppcommon {
uint32_t RandomInt(const int &min = 0, const int &max = 0);
double RandomDouble(const double &min = 0, const double &max = 1);

template <typename Unit = std::chrono::milliseconds>
inline auto CurrentTs() {
  auto now = std::chrono::high_resolution_clock::now();
  auto d = now.time_since_epoch();
  return std::chrono::duration_cast<Unit>(d).count();
}

struct DateInfo {
  std::tm *utc_time;

  inline int GetWeekDayNormal() { return utc_time->tm_wday; }
  inline int GetWeekDayShift() { return utc_time->tm_wday == 0 ? 6 : utc_time->tm_wday - 1; }
  inline int GetYear() { return utc_time->tm_year; }
  inline int GetYearDay() { return utc_time->tm_yday; }
  inline int GetMonth() { return utc_time->tm_mon; }
  inline int GetMonthDay() { return utc_time->tm_mday; }
  inline int GetHour() { return utc_time->tm_hour; }
};

inline DateInfo GetDateInfo(int64_t timestamp_ms = -1, int timezone_offset = 0) {
  if (timestamp_ms < 0) {
    timestamp_ms = CurrentTs();
  }
  auto ts_s = timestamp_ms / 1000 + timezone_offset * 3600;
  auto t = static_cast<std::time_t>(ts_s);
  std::tm *utc_time = std::gmtime(&t);
  std::mktime(utc_time);
  return {utc_time};
}

/// @brief get the day of the week, from 0 to 6
///   default timezone is utc 0 zone
int GetWeekDay(int64_t timestamp_ms = -1, int timezone_offset = 0);

template <typename E>
std::string WhatError(E ep) {
  try {
    std::rethrow_exception(ep);
  } catch (const std::exception &e) {
    return {e.what()};
  }
}

template <typename SA, typename SB>
bool StartsWith(const SA &compared, const SB &comparing) {
  return compared.size() >= comparing.size() && compared.compare(0, comparing.size(), comparing) == 0;
}

inline size_t BKDRHash(const char *str, size_t len) {
  unsigned int hash = 0;
  unsigned int seed = 131;
  const char *end = str + len;
  while (str < end) {
    char ch = (char)*str++;
    hash = hash * seed + ch;
  }
  return hash;
}

template <typename S>
size_t BKDRHash(S &str) {
  return BKDRHash(str.c_str(), str.size());
}

template <typename S>
inline bool IsNumber(const S &str) {
  for (const char c : str) {
    if (!std::isdigit(c)) {
      return false;
    }
  }
  return true;
}
}  // namespace cppcommon
