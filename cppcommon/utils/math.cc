#include "math.h"

#include <cmath>

namespace cppcommon {
double Sigmoid(double x) { return 1.0 / (1.0 + std::exp(-x)); }
}  // namespace cppcommon
