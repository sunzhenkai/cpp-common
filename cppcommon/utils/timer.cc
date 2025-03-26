#include "timer.h"

#include <chrono>
#include <mutex>
#include <thread>
#include <utility>

namespace cppcommon {
Timer::Timer(TimerTaskFunc task, std::chrono::milliseconds interval)
    : task_(std::move(task)), interval_(interval), running_(false) {}

void Timer::SetInterval(std::chrono::milliseconds interval) {
  interval_ = interval;
  Touch();
}

void Timer::Touch() {
  std::lock_guard<std::mutex> lock(cv_mutex_);
  cv_.notify_all();
}

void Timer::Start() {
  if (running_.exchange(true)) return;
  thread_ = std::thread([this] { run(); });
}

void Timer::Stop() {
  if (!running_.exchange(false)) return;
  {
    std::lock_guard<std::mutex> lock(cv_mutex_);
    cv_.notify_all();
  }
  if (thread_.joinable()) thread_.join();
}

void Timer::run() {
  while (running_) {
    auto start_time = std::chrono::steady_clock::now();
    if (task_) task_();
    auto elapsed = std::chrono::steady_clock::now() - start_time;
    if (elapsed < interval_) {
      std::unique_lock<std::mutex> lock(cv_mutex_);
      cv_.wait_for(lock, interval_ - elapsed, [this] { return !running_; });
    }
  }
}

Timer::~Timer() { Stop(); }
}  // namespace cppcommon
