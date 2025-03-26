/**
 * @file timer.h
 * @brief timer
 * @author zhenkai.sun
 * @date 2025-03-26 15:56:54
 */
#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
using namespace std::chrono_literals;

namespace cppcommon {
using TimerTaskFunc = std::function<void()>;

class Timer {
 public:
  Timer(TimerTaskFunc task, std::chrono::milliseconds interval = 60s);
  ~Timer();

  void Start();
  void Stop();
  void Touch();
  void SetInterval(std::chrono::milliseconds interval);

 private:
  void run();

 private:
  std::thread thread_;
  TimerTaskFunc task_;
  std::chrono::milliseconds interval_;
  std::atomic<bool> running_;
  std::mutex cv_mutex_;
  std::condition_variable cv_;
};
}  // namespace cppcommon
