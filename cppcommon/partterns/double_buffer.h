/**
 * @file double_buffer.h
 * @brief double buffer
 * @author zhenkai.sun
 * @date 2025-03-26 17:01:54
 */
#pragma once
#include <array>
#include <atomic>
#include <cstddef>
#include <mutex>

namespace cppcommon {
template <typename T>
class DoubleBuffer {
 public:
  explicit DoubleBuffer(const T& init_val = T{}) : buffers_{init_val, init_val}, write_index_(0) {}
  const T& GetRead() const noexcept { return buffers_[read_index()]; }

  T& GetWrite() {
    std::lock_guard<std::mutex> lock(write_mutex_);
    return buffers_[write_index()];
  }

  void Swap() {
    std::lock_guard<std::mutex> lock(write_mutex_);
    write_index_.store(1 - write_index(), std::memory_order_release);
  }

  template <typename F>
  void Commit(F&& write_op) {
    {
      std::lock_guard<std::mutex> lock(write_mutex_);
      write_op(buffers_[write_index()]);
    }
    Swap();
  }

 private:
  size_t read_index() const { return 1 - write_index_.load(std::memory_order_acquire); }
  size_t write_index() const { return write_index_.load(std::memory_order_release); }

 private:
  std::array<T, 2> buffers_;
  std::atomic<size_t> write_index_;
  mutable std::mutex write_mutex_;
};
}  // namespace cppcommon
