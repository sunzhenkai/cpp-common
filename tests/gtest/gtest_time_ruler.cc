#include <thread>

#include "cppcommon/utils/time_ruler.h"
#include "gtest/gtest.h"
using namespace std::chrono_literals;

TEST(TimeRuler, A) {
  cppcommon::TimeRuler tr;
  std::this_thread::sleep_for(100ms);
  std::cout << tr.Elapsed() << "ms" << std::endl;
  tr.Stop();
  std::cout << tr.Elapsed() << "ms" << std::endl;
  std::cout << tr.ElapsedNow() << "ms" << std::endl;
  std::cout << tr.ToString() << std::endl;

  tr.Reset();
  std::this_thread::sleep_for(50ms);
  tr.Stop();
  std::cout << tr.Elapsed() << "ms" << std::endl;
  std::cout << tr.ElapsedNow() << "ms" << std::endl;
  std::this_thread::sleep_for(20ms);
  std::cout << tr.ElapsedNow() << "ms" << std::endl;
  std::cout << tr.ToString() << std::endl;
}
