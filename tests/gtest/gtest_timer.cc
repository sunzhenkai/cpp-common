#include <spdlog/spdlog.h>

#include <thread>

#include "cppcommon/utils/timer.h"
#include "gtest/gtest.h"

TEST(Timer, A) {
  cppcommon::Timer timer([]() { spdlog::info("run once"); }, 3s);
  timer.Start();
  std::this_thread::sleep_for(1.5s);
  spdlog::info("Touch");
  timer.Touch();
  std::this_thread::sleep_for(5s);
  spdlog::info("SetInterval");
  timer.SetInterval(2s);
  std::this_thread::sleep_for(10s);
  spdlog::info("Done");
}
