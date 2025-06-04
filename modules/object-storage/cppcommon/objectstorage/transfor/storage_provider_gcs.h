/**
 * @file storage_provider_gcs.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-06-04 10:47:15
 */
#pragma once
#include <memory>
#include <string>
#include <vector>

#include "cppcommon/objectstorage/transfor/storage_provider.h"
#include "google/cloud/storage/client.h"

namespace cppcommon::os {
namespace gcs = google::cloud::storage;
class GcsStorageProvider : public StorageProvider {
 public:
  explicit GcsStorageProvider(StorageProviderOptions &&options);
  GcsStorageProvider();

  std::vector<std::string> List(const std::string &bucket, const std::string &path) override;
  absl::Status Upload(const std::string &bucket, const std::string &object_key,
                      const std::string &local_file_path) override;

 private:
  std::shared_ptr<gcs::Client> client_;
};
}  // namespace cppcommon::os
