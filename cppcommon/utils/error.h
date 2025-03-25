/**
 * @file error.h
 * @brief exception processing functions
 * @author zhenkai.sun
 * @date 2025-03-19 15:42:40
 */
#pragma once
#include <iostream>

namespace cppcommon {
template <typename T> void Assert(bool condition, T message) {
  if (!condition) {
    std::cerr << "Assertion failed: " << message << std::endl;
    std::abort();
  }
}
}; // namespace cppcommon
