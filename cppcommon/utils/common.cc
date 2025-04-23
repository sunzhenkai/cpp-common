// sys
#include <climits>
#include <random>
// self
#include "common.h"

namespace cppcommon {
std::random_device gRandomDev;
std::mt19937 gRandomGenerator(gRandomDev());
std::uniform_int_distribution<std::mt19937::result_type> gRandomDist(0, INT_MAX);

uint32_t RandomInt(const int &min, const int &max) {
  if (min == max && min == 0) {
    return (uint32_t)gRandomDist(gRandomGenerator);
  } else {
    std::uniform_int_distribution<std::mt19937::result_type> dist(min, max);
    return (uint32_t)dist(gRandomGenerator);
  }
}

double RandomDouble(const double &min, const double &max) {
  std::uniform_real_distribution<> dist(min, max);
  return dist(gRandomGenerator);
}

int GetWeekDay(int64_t timestamp_ms, int timezone_offset) {
  auto di = GetDateInfo(timestamp_ms, timezone_offset);
  // default tm_wday: sunday ~ saturday
  return di.utc_time->tm_wday == 0 ? 6 : di.utc_time->tm_wday - 1;
}
}  // namespace cppcommon
