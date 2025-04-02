#include "time_ruler.h"

namespace cppcommon {
TimeRuler::TimeRuler() { start_ = clock::now(); }

void TimeRuler::Stop() { end_ = clock::now(); }

void TimeRuler::Reset() { start_ = clock::now(); }
}  // namespace cppcommon
