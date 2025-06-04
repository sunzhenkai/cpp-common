#include "storage_provider_gcs.h"

#include <alibabacloud/oss/OssClient.h>
#include <alibabacloud/oss/auth/CredentialsProvider.h>
#include <google/cloud/storage/client_options.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "absl/status/status.h"
#include "cppcommon/objectstorage/transfor/storage_provider.h"
#include "spdlog/spdlog.h"

namespace cppcommon::os {
GcsStorageProvider::GcsStorageProvider(StorageProviderOptions &&options) {
  auto co = gcs::ClientOptions::CreateDefaultClientOptions().value();
  client_ = std::make_shared<gcs::Client>(std::move(co));
}

GcsStorageProvider::GcsStorageProvider() : GcsStorageProvider(StorageProviderOptions{}) {}

absl::StatusOr<FileList> GcsStorageProvider::List(const std::string &bucket, const std::string &path) {
  std::vector<std::string> keys;
  for (auto &&object_metadata : client_->ListObjects(bucket, gcs::Prefix(path))) {
    if (!object_metadata) {
      std::cerr << "[GCS::List] Error: " << object_metadata.status().message() << "\n";
      continue;
    }
    keys.emplace_back(object_metadata->name());
  }
  return keys;
}

absl::Status GcsStorageProvider::Upload(const TransferMeta &m) {
  std::ifstream source(m.local_file_path, std::ios::binary);
  if (!source.is_open()) {
    return absl::NotFoundError(absl::StrFormat("Cannot open file: %s", m.local_file_path));
  }
  auto writer = client_->WriteObject(m.bucket, m.remote_file_path);
  writer << source.rdbuf();
  source.close();
  writer.Close();
  return absl::OkStatus();
}

absl::Status GcsStorageProvider::DownloadFile(const TransferMeta &m) {
  auto &lp = m.local_file_path;
  ExpectOrInternal(!lp.empty(), FMT("destination path should not be empty. [{}]", m.ToString()));
  ExpectOrInternal(lp[lp.size() - 1] != '/', FMT("destination file path should not end with '/'. [{}]", m.ToString()));

  // 1. ensure local dest path
  OkOrRet(EnsureLocalPath(fs::path(m.local_file_path), m.overwrite));

  // 2. download file
  auto writer = client_->ReadObject(m.bucket, m.remote_file_path);
  if (!writer) {
    return absl::InternalError(FMT("Failed to read GCS object. [bucket={}, path={}]", m.bucket, m.remote_file_path));
  }

  std::ofstream outFile(m.local_file_path, std::ios::binary);
  if (!outFile) {
    return absl::InternalError(FMT("Failed to open local file. [path={}]", m.local_file_path));
  }

  outFile << writer.rdbuf();
  if (!outFile) {
    return absl::InternalError(FMT("Failed to write to local file. [path={}]", m.local_file_path));
  }

  return absl::OkStatus();
}

absl::StatusOr<FilePathList> GcsStorageProvider::Download(const TransferMeta &meta) {
  std::filesystem::path base = std::filesystem::path(meta.local_file_path) / std::filesystem::path(meta.file_name);
  std::vector<std::filesystem::path> result;
  // list files
  google::cloud::storage::ListObjectsReader reader =
      client_->ListObjects(meta.bucket, google::cloud::storage::Prefix(meta.remote_file_path));

  for (auto const &object : reader) {
    if (!object) {
      spdlog::error("Failed to list GCS objects. [err={}]", object.status().message());
      continue;
    }

    auto key = object->name();
    auto cur = base / std::filesystem::path(key.substr(meta.remote_file_path.size()));

    if (key.back() == '/') continue;

    auto dm = TransferMeta{.bucket = meta.bucket, .remote_file_path = key, .local_file_path = cur};
    auto s = DownloadFile(dm);
    if (!s.ok()) {
      spdlog::error("Download gcs object failed. [info={}, error={}]", dm.ToString(), s.ToString());
    } else {
      spdlog::info("Download gcs object success. [info={}]", dm.ToString());
    }
    result.emplace_back(cur);
  }
  return result;
}
}  // namespace cppcommon::os
