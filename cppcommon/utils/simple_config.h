/**
 * @file simple_config.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-04-29 12:54:36
 */
#pragma once
#include <cstdint>
#include <cstdlib>
#include <string>
#include <unordered_map>

namespace cppcommon {
/**
 * config format:
 *  - ';' split kvs
 *  - ':' split kv
 *
 * bool:
 * - true: 'true', 'True', 'TRUE', '1'
 * - false: 'false', 'False', 'FALSE', '0'
 *
 * example:
 * - 'a:b;c:1;pi:3.14'
 */
class SimpleConfig {
 public:
  explicit SimpleConfig(const std::string &config, bool quiet = true);

  inline const std::unordered_map<std::string, std::string> &Data() const { return data_; }

  inline const std::string &Get(const std::string &k, const std::string &dft) const {
    auto it = data_.find(k);
    if (it != data_.end()) {
      return it->second;
    } else {
      return dft;
    }
  }

  inline int64_t GetInt64(const std::string &k, int64_t dft) {
    auto &v = Get(k, "");
    if (v.empty()) return dft;
    try {
      return std::stoll(v);
    } catch (...) {
    }
    return dft;
  }

  inline double GetDouble(const std::string &k, double dft) {
    auto &v = Get(k, "");
    if (v.empty()) return dft;
    try {
      return std::stod(v);
    } catch (...) {
    }
    return dft;
  }

  inline bool GetBoolean(const std::string &k, bool dft) {
    auto &v = Get(k, "");
    if (v.empty()) return dft;
    if (v == "true" || v == "True" || v == "TRUE" || v == "1") {
      return true;
    }
    if (v == "false" || v == "False" || v == "FALSE" || v == "0") {
      return false;
    }
    return dft;
  }

  inline size_t Size() { return data_.size(); }

  inline bool HasError() { return has_errors_; }

  std::string ToString();

 private:
  std::unordered_map<std::string, std::string> data_;
  bool has_errors_{false};
};
}  // namespace cppcommon
