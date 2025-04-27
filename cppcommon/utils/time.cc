#include "cppcommon/utils/time.h"

namespace cppcommon {
int GetWeekDay(int64_t timestamp_ms, int timezone_offset) {
  auto di = GetDateInfo(timestamp_ms, timezone_offset);
  // default tm_wday: sunday ~ saturday
  return di.tm->tm_wday == 0 ? 6 : di.tm->tm_wday - 1;
}
}  // namespace cppcommon
