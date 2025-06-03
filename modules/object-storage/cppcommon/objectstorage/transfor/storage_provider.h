/**
 * @file trans_provider.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-06-03 11:08:55
 */
#pragma once
#include <string>
#include <vector>

#include "absl/status/status.h"

namespace cppcommon::os {
struct StorageProviderOptions {
  std::string access_key_id;
  std::string access_key_secret;
  std::string region;
  std::string endpoint;
};

class StorageProvider {
 public:
  virtual std::vector<std::string> List(const std::string &bucket, const std::string &path) = 0;
  virtual absl::Status Upload(const std::string &bucket, const std::string &object_key,
                              const std::string &local_file_path) = 0;
};
}  // namespace cppcommon::os
