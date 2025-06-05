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
#include "absl/status/statusor.h"
#include "cppcommon/extends/abseil/absl.h"
#include "cppcommon/extends/fmt/fmt.h"
#include "cppcommon/utils/str.h"

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

  inline std::string ToString() const {
    return FMT("[bucket={}, remote_file_path={}, local_file_path={}, overwrite={}]", bucket, remote_file_path,
               local_file_path, overwrite);
  }
};

namespace fs = std::filesystem;
using FileList = std::vector<std::string>;
using FilePathList = std::vector<std::filesystem::path>;

class StorageProvider {
 public:
  virtual absl::StatusOr<FileList> List(const std::string &bucket, const std::string &path) = 0;
  virtual absl::Status Upload(const TransferMeta &meta) = 0;
  virtual absl::Status DownloadFile(const TransferMeta &meta) = 0;
  virtual absl::StatusOr<FilePathList> Download(const TransferMeta &meta) = 0;

 protected:
  absl::Status EnsureLocalPath(const fs::path &p, bool overwrite = false);
  absl::Status PreDownloadFile(const TransferMeta &meta);
};

inline absl::Status StorageProvider::EnsureLocalPath(const fs::path &p, bool overwrite) {
  auto parent = p.parent_path().string();
  if (fs::exists(p)) {
    ExpectOrInternal(overwrite, "destination file exists and overwrite is disabled.");
    ExpectOrInternal(fs::is_regular_file(p), "destination is not regular file.");
  } else if (fs::exists(p.parent_path())) {
    ExpectOrInternal(fs::is_directory(p.parent_path()), "destination parent path should be directory.");
  } else if (!parent.empty() && parent != ".") {
    ExpectOrInternal(fs::create_directories(p.parent_path()), "create directory failed.");
  }
  return absl::OkStatus();
}

inline absl::Status StorageProvider::PreDownloadFile(const TransferMeta &m) {
  auto &lp = m.local_file_path;
  ExpectOrInternal(!lp.empty(), FMT("destination path should not be empty. [{}]", m.ToString()));
  ExpectOrInternal(lp.back() != '/', FMT("destination file path should not end with '/'. [{}]", m.ToString()));
  OkOrRet(EnsureLocalPath(fs::path(m.local_file_path), m.overwrite));
  return absl::OkStatus();
}

inline std::string GetObjLocalFilePath(const std::string &remove_file_path, const std::string &local_file_path,
                                       const std::string &obj) {
  auto base = fs::path(local_file_path);
  auto remote_file_path_prefix = cppcommon::TrimSuffix(remove_file_path, "/");
  auto relative_path = obj.substr(remove_file_path.size() + 1);
  auto cur = base.empty() ? fs::path(relative_path) : base / fs::path(relative_path);
  return cur.string();
}
}  // namespace cppcommon::os
