#include "simple_config.h"

#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "str.h"
#include "to_str.h"

namespace cppcommon {
SimpleConfig::SimpleConfig(const std::string &config, bool quiet) {
  std::vector<std::string> kvs;
  StringSplit(kvs, config, ';');

  for (auto &item : kvs) {
    auto pos = item.find(':');
    if (pos != std::string::npos) {
      data_[item.substr(0, pos)] = item.substr(pos + 1, item.size() - pos);
    } else {
      has_errors_ = true;
      if (!quiet) {
        std::cerr << "[SimpleConfig] parse kv failed. kv=" << item << std::endl;
      }
    }
  }
}

std::string SimpleConfig::ToString() { return cppcommon::ToString(data_); }
}  // namespace cppcommon
