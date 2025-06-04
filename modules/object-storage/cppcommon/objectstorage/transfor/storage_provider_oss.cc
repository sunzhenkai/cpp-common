#include "storage_provider_oss.h"

#include <alibabacloud/oss/OssClient.h>
#include <alibabacloud/oss/auth/CredentialsProvider.h>

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "absl/status/status.h"
#include "cppcommon/objectstorage/transfor/storage_provider.h"
#include "cppcommon/utils/os.h"

namespace cppcommon::os {
StorageProviderOptions GetOssOptionsFromEnv() {
  StorageProviderOptions options;
  options.access_key_id = cppcommon::GetEnv("OSS_ACCESS_KEY_ID", "");
  options.access_key_secret = cppcommon::GetEnv("OSS_ACCESS_KEY_SECRET", "");
  options.region = cppcommon::GetEnv("OSS_REGION", "");
  options.endpoint = cppcommon::GetEnv("OSS_ENDPOINT", "");
  return options;
}

OssStorageProvider::OssStorageProvider(StorageProviderOptions &&options) {
  oss::ClientConfiguration conf;
  conf.signatureVersion = oss::SignatureVersionType::V4;
  auto credentialsProvider =
      std::make_shared<oss::SimpleCredentialsProvider>(options.access_key_id, options.access_key_secret);
  client_ = std::make_shared<oss::OssClient>(options.endpoint, credentialsProvider, conf);
  client_->SetRegion(options.region);
}

OssStorageProvider::OssStorageProvider() : OssStorageProvider(GetOssOptionsFromEnv()) {}

std::vector<std::string> OssStorageProvider::List(const std::string &bucket, const std::string &path) {
  std::vector<std::string> keys;

  oss::ListObjectsRequest request(bucket);
  request.setPrefix(path);
  request.setDelimiter("/");

  auto outcome = client_->ListObjects(request);
  if (!outcome.isSuccess()) {
    std::cerr << "[OSS::List] Error: " << outcome.error().Message() << "\n";
    return keys;
  }

  for (const auto &obj : outcome.result().ObjectSummarys()) {
    keys.emplace_back(obj.Key());
  }
  return keys;
}

absl::Status OssStorageProvider::Upload(const std::string &bucket, const std::string &object_key,
                                        const std::string &local_file_path) {
  try {
    auto outcome = client_->PutObject(bucket, object_key, local_file_path);
    if (!outcome.isSuccess()) {
      return absl::InternalError(
          absl::StrFormat("OSS Upload failed: code=%s, message=%s", outcome.error().Code(), outcome.error().Message()));
    }
    return absl::OkStatus();
  } catch (const std::exception &e) {
    return absl::InternalError(e.what());
  }
}
}  // namespace cppcommon::os
