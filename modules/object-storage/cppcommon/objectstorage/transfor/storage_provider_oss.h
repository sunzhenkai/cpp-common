/**
 * @file storage_provider_oss.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-06-03 11:14:25
 */
#pragma once
#include <memory>
#include <string>
#include <vector>

#include "alibabacloud/oss/OssClient.h"
#include "cppcommon/objectstorage/transfor/storage_provider.h"

namespace cppcommon::os {
namespace oss = AlibabaCloud::OSS;

class OssStorageProvider : public StorageProvider {
 public:
  explicit OssStorageProvider(StorageProviderOptions &&options);
  OssStorageProvider();

  absl::StatusOr<FileList> List(const std::string &bucket, const std::string &path) override;
  absl::Status Upload(const TransferMeta &m) override;
  absl::Status DownloadFile(const TransferMeta &meta) override;
  absl::StatusOr<FilePathList> Download(const TransferMeta &meta) override;

 private:
  std::shared_ptr<oss::OssClient> client_;
};
}  // namespace cppcommon::os
