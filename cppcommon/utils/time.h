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
using days = std::chrono::duration<int, std::ratio<86400>>;

template <typename Unit = std::chrono::milliseconds>
inline auto CurrentTs() {
  auto now = std::chrono::high_resolution_clock::now();
  auto d = now.time_since_epoch();
  return std::chrono::duration_cast<Unit>(d).count();
}

inline auto CurrentTsMs() { return CurrentTs<std::chrono::milliseconds>(); }

template <typename U>
inline int DurationCount(const std::chrono::duration<int64_t>& d) {
  return static_cast<int>(std::chrono::duration_cast<U>(d).count());
}

inline int DurationDays(const std::chrono::duration<int64_t>& d) {
  return static_cast<int>(std::chrono::duration_cast<days>(d).count());
}

inline int DurationHours(const std::chrono::duration<int64_t>& d) {
  return static_cast<int>(std::chrono::duration_cast<std::chrono::hours>(d).count());
}

inline int DurationMinutes(const std::chrono::duration<int64_t>& d) {
  return static_cast<int>(std::chrono::duration_cast<std::chrono::minutes>(d).count());
}

inline int DurationSeconds(const std::chrono::duration<int64_t>& d) {
  return static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(d).count());
}

struct DateInfo {
  std::shared_ptr<std::tm> tm;
  explicit DateInfo(int64_t timestamp_ms = -1, int timezone_offset = 0);
  explicit DateInfo(std::shared_ptr<std::tm> tm);

  // since sunday, [0, 6]. sunday: 0, monday~staurday: 1~6
  inline int GetWeekDay() { return tm->tm_wday; }
  // since monday, [0, 6]. monday~sunday: 0~6
  inline int GetWeekDayShift() { return tm->tm_wday == 0 ? 6 : tm->tm_wday - 1; }
  // since 1900
  inline int GetYear() { return tm->tm_year; }
  inline int GetHumanYear() { return 1900 + tm->tm_year; }
  // [0, 365]
  inline int GetYearDay() { return tm->tm_yday; }
  // [0, 11]
  inline int GetMonth() { return tm->tm_mon; }
  // [1, 12]
  inline int GetHumanMonth() { return tm->tm_mon + 1; }
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

  inline DateInfo Apply(int years = 0, int months = 0, int days = 0, int hours = 0, int minutes = 0, int seconds = 0) {
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
    return DateInfo(std::make_shared<std::tm>(*ntm));
  }
};

inline DateInfo GetDateInfo(int64_t timestamp_ms = -1, int timezone_offset = 0) {
  return DateInfo(timestamp_ms, timezone_offset);
}

/// @brief get the day of the week, from 0 to 6
///   default timezone is utc 0 zone
int GetWeekDay(int64_t timestamp_ms = -1, int timezone_offset = 0);
}  // namespace cppcommon
