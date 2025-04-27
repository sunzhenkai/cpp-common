/**
 * @file time.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-04-27 14:40:48
 */
#pragma once
#include <chrono>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

namespace cppcommon {
template <typename Unit = std::chrono::milliseconds>
inline auto CurrentTs() {
  auto now = std::chrono::high_resolution_clock::now();
  auto d = now.time_since_epoch();
  return std::chrono::duration_cast<Unit>(d).count();
}

struct DateInfo {
  std::shared_ptr<std::tm> tm;

  // since sunday, [0, 6]. sunday: 0, monday~staurday: 1~6
  inline int GetWeekDay() { return tm->tm_wday; }
  // since monday, [0, 6]. monday~sunday: 0~6
  inline int GetWeekDayShift() { return tm->tm_wday == 0 ? 6 : tm->tm_wday - 1; }
  // since 1900
  inline int GetYear() { return tm->tm_year; }
  // [0, 365]
  inline int GetYearDay() { return tm->tm_yday; }
  // [0, 11]
  inline int GetMonth() { return tm->tm_mon; }
  // [1, 31]
  inline int GetMonthDay() { return tm->tm_mday; }
  // [0, 23]
  inline int GetHour() { return tm->tm_hour; }
  // [0, 59]
  inline int GetMinute() { return tm->tm_min; }
  // [0, 60]
  inline int GetSecond() { return tm->tm_sec; }

  inline std::string Format(const char* f) {
    std::ostringstream ss;
    ss << std::put_time(tm.get(), f);
    return ss.str();
  }

  inline DateInfo Apply(int years, int months, int days, int hours, int minutes, int seconds) {
    std::shared_ptr<std::tm> new_tm = std::make_shared<std::tm>(*tm);
    new_tm->tm_year += years;
    new_tm->tm_mon += months;
    new_tm->tm_mday += days;
    new_tm->tm_hour += hours;
    new_tm->tm_min += minutes;
    new_tm->tm_sec += seconds;

    auto tt = timegm(new_tm.get());
    if (tt == -1) {
      throw std::runtime_error("[DateInfo::Apply] make time failed");
    }
    auto ntm = std::gmtime(&tt);
    return {.tm = std::make_shared<std::tm>(*ntm)};
  }
};

inline DateInfo GetDateInfo(int64_t timestamp_ms = -1, int timezone_offset = 0) {
  if (timestamp_ms < 0) {
    timestamp_ms = CurrentTs();
  }
  auto ts_s = timestamp_ms / 1000 + timezone_offset * 3600;
  auto t = static_cast<std::time_t>(ts_s);
  std::tm* tm = std::gmtime(&t);
  return DateInfo{.tm = std::make_shared<std::tm>(*tm)};
}

/// @brief get the day of the week, from 0 to 6
///   default timezone is utc 0 zone
int GetWeekDay(int64_t timestamp_ms = -1, int timezone_offset = 0);
}  // namespace cppcommon
