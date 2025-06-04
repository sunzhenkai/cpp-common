/**
 * @file error.h
 * @brief exception processing functions
 * @author zhenkai.sun
 * @date 2025-03-19 15:42:40
 */
#pragma once
#include <iostream>

namespace cppcommon {
#ifndef EXPECT
#define EXPECT(expr, message) \
  if (!(expr)) throw std::runtime_error(message);
#endif

template <typename E, typename T>
inline void Expect(const E &expr, const T &message) {
  if (!expr) {
    throw std::runtime_error(message);
  }
}

template <typename T>
void Assert(bool condition, T message) {
  if (!condition) {
    std::cerr << "Assertion failed: " << message << std::endl;
    std::abort();
  }
}
};  // namespace cppcommon
