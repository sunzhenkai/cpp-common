/**
 * @file time_ruler.h
 * @brief time ruler
 * @author zhenkai.sun
 * @date 2025-04-02 10:18:00
 */

#include <chrono>
#include <cstdint>
#include <ratio>
#include <sstream>
namespace cppcommon {
class TimeRuler {
 public:
  using clock = std::chrono::high_resolution_clock;
  TimeRuler();

  void Reset();
  void Stop();

  template <typename Unit = std::chrono::milliseconds>
  int64_t Elapsed() {
    //< if Stop not called
    if (end_ < start_) {
      return ElapsedNow();
    } else {
      return std::chrono::duration_cast<Unit>(end_ - start_).count();
    }
  }

  template <typename Unit = std::chrono::milliseconds>
  int64_t ElapsedNow() {
    return std::chrono::duration_cast<Unit>(clock::now() - start_).count();
  }

  template <typename Unit = std::chrono::milliseconds>
  std::string ToString() {
    auto duration = Elapsed();
    std::stringstream ss;
    ss << duration << unit_suffix<Unit>();
    return ss.str();
  }

 private:
  clock::time_point start_, end_{};

  template <typename Unit>
  const char* unit_suffix() const {
    if constexpr (std::is_same_v<Unit, std::chrono::nanoseconds>)
      return "ns";
    else if (std::is_same_v<Unit, std::chrono::microseconds>)
      return "μs";
    else if (std::is_same_v<Unit, std::chrono::milliseconds>)
      return "ms";
    else if (std::is_same_v<Unit, std::chrono::seconds>)
      return "s";
    else if (std::is_same_v<Unit, std::chrono::minutes>)
      return "m";
    else if (std::is_same_v<Unit, std::chrono::hours>)
      return "h";
    else
      return "";
  }
};
}  // namespace cppcommon
