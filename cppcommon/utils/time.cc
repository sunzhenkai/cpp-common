#include "cppcommon/utils/time.h"

#include <memory>
#include <utility>

namespace cppcommon {
int GetWeekDay(int64_t timestamp_ms, int timezone_offset) {
  auto di = GetDateInfo(timestamp_ms, timezone_offset);
  // default tm_wday: sunday ~ saturday
  return di.tm->tm_wday == 0 ? 6 : di.tm->tm_wday - 1;
}

DateInfo::DateInfo(int64_t timestamp_ms, int timezone_offset) {
  if (timestamp_ms < 0) {
    timestamp_ms = CurrentTs();
  }
  auto ts_s = timestamp_ms / 1000 + timezone_offset * 3600;
  auto t = static_cast<std::time_t>(ts_s);
  std::tm* ttm = std::gmtime(&t);
  tm = std::make_shared<std::tm>(*ttm);
}

DateInfo::DateInfo(std::shared_ptr<std::tm> ttm) : tm(std::move(ttm)) {}
}  // namespace cppcommon
