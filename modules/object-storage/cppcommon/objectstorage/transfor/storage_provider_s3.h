/**
 * @file storage_provider_oss.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-06-03 11:14:25
 */
#pragma once
#include <aws/s3/S3Client.h>

#include <memory>
#include <string>
#include <vector>

#include "cppcommon/objectstorage/transfor/storage_provider.h"

namespace cppcommon::os {

class S3StorageProvider : public StorageProvider {
 public:
  explicit S3StorageProvider(StorageProviderOptions &&options);
  S3StorageProvider();

  std::vector<std::string> List(const std::string &bucket, const std::string &path) override;
  absl::Status Upload(const std::string &bucket, const std::string &object_key,
                      const std::string &local_file_path) override;

 private:
  std::shared_ptr<Aws::S3::S3Client> client_;
};
}  // namespace cppcommon::os
