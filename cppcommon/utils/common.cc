// sys
#include <climits>
#include <cstdint>
#include <random>
// self
#include "common.h"

namespace cppcommon {
std::random_device gRandomDev;
std::mt19937 gRandomGenerator(gRandomDev());
std::uniform_int_distribution<std::mt19937::result_type> gRandomDist(0, INT_MAX);

uint32_t RandomInt(const int &min, const int &max) {
  if (min == max && min == 0) {
    return static_cast<uint32_t>(gRandomDist(gRandomGenerator));
  } else {
    std::uniform_int_distribution<std::mt19937::result_type> dist(min, max);
    return static_cast<uint32_t>(dist(gRandomGenerator));
  }
}

double RandomDouble(const double &min, const double &max) {
  std::uniform_real_distribution<> dist(min, max);
  return dist(gRandomGenerator);
}
}  // namespace cppcommon
