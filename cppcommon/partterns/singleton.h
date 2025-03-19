/**
 * @file singleton.h
 * @brief singleton base class
 * @author zhenkai.sun
 * @date 2025-03-19 16:56:41
 */
#include <iostream>

namespace cppcommon {
template <class T> class Singleton {
private:
  Singleton() = default;
  virtual ~Singleton() = default;
  Singleton(const Singleton &) = delete;
  Singleton &operator=(const Singleton &) = delete;

public:
  static T &Instance() {
    static T instance;
    return instance;
  }
};
} // namespace cppcommon
