/**
 * @file common.c
 * @brief
 * @author zhenkai.sun
 * @date 2025-03-25 21:50:05
 */
#pragma once
#include <exception>
#include <random>

namespace cppcommon {
uint32_t RandomInt(const int &min = 0, const int &max = 0);
double RandomDouble(const double &min = 0, const double &max = 1);

template <typename E>
std::string WhatError(E ep) {
  try {
    std::rethrow_exception(ep);
  } catch (const std::exception &e) {
    return {e.what()};
  }
}

template <typename SA, typename SB>
bool StartsWith(const SA &compared, const SB &comparing) {
  return compared.size() >= comparing.size() && compared.compare(0, comparing.size(), comparing) == 0;
}

inline size_t BKDRHash(const char *str, size_t len) {
  unsigned int hash = 0;
  unsigned int seed = 131;
  const char *end = str + len;
  while (str < end) {
    char ch = (char)*str++;
    hash = hash * seed + ch;
  }
  return hash;
}

template <typename S>
size_t BKDRHash(S &str) {
  return BKDRHash(str.c_str(), str.size());
}

template <typename S>
inline bool IsNumber(const S &str) {
  for (const char c : str) {
    if (!std::isdigit(c)) {
      return false;
    }
  }
  return true;
}
}  // namespace cppcommon
