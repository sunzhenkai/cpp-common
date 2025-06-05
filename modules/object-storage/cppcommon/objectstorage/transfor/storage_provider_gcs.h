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
  explicit GcsStorageProvider(const std::string &service_account_json_string);
  GcsStorageProvider();

  absl::StatusOr<FileList> List(const std::string &bucket, const std::string &path) override;
  absl::Status Upload(const TransferMeta &m) override;
  absl::Status DownloadFile(const TransferMeta &meta) override;
  absl::StatusOr<FilePathList> Download(const TransferMeta &meta) override;

 private:
  std::shared_ptr<gcs::Client> client_;
};
}  // namespace cppcommon::os
