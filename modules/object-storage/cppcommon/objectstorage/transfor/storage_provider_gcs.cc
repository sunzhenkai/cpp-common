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

namespace cppcommon::os {
GcsStorageProvider::GcsStorageProvider(StorageProviderOptions &&options) {
  auto co = gcs::ClientOptions::CreateDefaultClientOptions().value();
  client_ = std::make_shared<gcs::Client>(std::move(co));
}

GcsStorageProvider::GcsStorageProvider() : GcsStorageProvider(StorageProviderOptions{}) {}

std::vector<std::string> GcsStorageProvider::List(const std::string &bucket, const std::string &path) {
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

absl::Status GcsStorageProvider::Upload(const std::string &bucket, const std::string &object_key,
                                        const std::string &local_file_path) {
  std::ifstream source(local_file_path, std::ios::binary);
  if (!source.is_open()) {
    return absl::NotFoundError(absl::StrFormat("Cannot open file: %s", local_file_path));
  }
  auto writer = client_->WriteObject(bucket, object_key);
  writer << source.rdbuf();
  source.close();
  writer.Close();
  return absl::OkStatus();
}
}  // namespace cppcommon::os
