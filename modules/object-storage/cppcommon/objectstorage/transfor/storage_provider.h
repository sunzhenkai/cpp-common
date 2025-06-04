/**
 * @file trans_provider.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-06-03 11:08:55
 */
#pragma once
#include <filesystem>
#include <string>
#include <vector>

#include "absl/status/status.h"
#include "cppcommon/extends/abseil/absl.h"
#include "cppcommon/extends/fmt/fmt.h"

namespace cppcommon::os {
struct StorageProviderOptions {
  std::string access_key_id;
  std::string access_key_secret;
  std::string region;
  std::string endpoint;
};

struct TransferMeta {
  std::string bucket;
  std::string remote_file_path;
  std::string local_file_path;
  bool overwrite{true};
  std::string file_name;

  inline std::string ToString() const {
    return FMT("bucket={}, remote_file_path={}, local_file_path={}, file_name={}, overwrite={}", bucket,
               remote_file_path, local_file_path, file_name, overwrite);
  }
};

namespace fs = std::filesystem;
class StorageProvider {
 public:
  virtual std::vector<std::string> List(const std::string &bucket, const std::string &path) = 0;
  virtual absl::Status Upload(const TransferMeta &meta) = 0;
  virtual absl::Status DownloadFile(const TransferMeta &meta) = 0;
  virtual std::vector<std::filesystem::path> Download(const TransferMeta &meta) = 0;

 protected:
  absl::Status EnsureLocalPath(const fs::path &p, bool overwrite = false) {
    if (fs::exists(p)) {
      ExpectOrInternal(overwrite, "destination file exists and overwrite is disabled.");
      ExpectOrInternal(fs::is_regular_file(p), "destination is not regular file.");
    } else if (fs::exists(p.parent_path())) {
      ExpectOrInternal(fs::is_directory(p.parent_path()), "destination parent path should be directory.");
    } else {
      ExpectOrInternal(fs::create_directories(p.parent_path()), "create directory failed.");
    }
    return absl::OkStatus();
  }
};
}  // namespace cppcommon::os
