#ifndef RTCFG_EXCEPTIONS_H
#define RTCFG_EXCEPTIONS_H

#include <exception>
#include <utility>

#include "defines.h"

namespace cxxcommon {
class NetworkException : public std::exception {
 private:
  int code_;
  Str message_;

 public:
  NetworkException(int code, const char* message)
      : code_(code), message_(message){};
  NetworkException(int code, Str message)
      : code_(code), message_(std::move(message)){};
  ~NetworkException() override = default;
  const char* what() const noexcept override { return message_.c_str(); }
  int code() const { return code_; }
};

class ReadTimeoutException : public std::exception {
  Str message_;

 public:
  explicit ReadTimeoutException(Str message) : message_(MOVE(message)) {}
  const char* what() const noexcept override { return message_.c_str(); }
};

class RTException : public std::exception {
 private:
  Str message_;

 public:
  explicit RTException(Str message) : message_(std::move(message)) {}

  const char* what() const noexcept override { return message_.c_str(); }
};
}  // namespace cxxcommon

#endif  // RTCFG_EXCEPTIONS_H
