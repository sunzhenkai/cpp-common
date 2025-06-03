#include "storage_provider_s3.h"

#include <alibabacloud/oss/OssClient.h>
#include <alibabacloud/oss/auth/CredentialsProvider.h>
#include <aws/core/Aws.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/ListBucketsRequest.h>
#include <aws/s3/model/ListObjectsRequest.h>
#include <aws/s3/model/PutObjectRequest.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "absl/status/status.h"
#include "cppcommon/utils/os.h"
#include "fmt/format.h"

namespace cppcommon::os {
StorageProviderOptions GetS3OptionsFromEnv() {
  StorageProviderOptions options;
  options.access_key_id = cppcommon::GetEnv("AWS_ACCESS_KEY_ID", "");
  options.access_key_secret = cppcommon::GetEnv("AWS_SECRET_ACCESS_KEY", "");
  options.region = cppcommon::GetEnv("AWS_REGION", "");
  options.endpoint = cppcommon::GetEnv("AWS_ENDPOINT", "");
  return options;
}

S3StorageProvider::S3StorageProvider(StorageProviderOptions &&options) {
  Aws::Client::ClientConfiguration config;
  if (!options.endpoint.empty()) {
    config.endpointOverride = options.endpoint;
  }
  if (!options.region.empty()) {
    config.region = options.region;
  }
  Aws::Auth::AWSCredentials credentials(options.access_key_id, options.access_key_secret);
  client_ = std::make_shared<Aws::S3::S3Client>(credentials, nullptr, config);
}

S3StorageProvider::S3StorageProvider() : S3StorageProvider(GetS3OptionsFromEnv()) {}

std::vector<std::string> S3StorageProvider::List(const std::string &bucket, const std::string &path) {
  Aws::S3::Model::ListObjectsRequest request;
  request.WithBucket(bucket).WithPrefix(path).WithDelimiter("/");

  std::vector<std::string> ret;
  auto outcome = client_->ListObjects(request);
  if (outcome.IsSuccess()) {
    for (const auto &obj : outcome.GetResult().GetContents()) {
      ret.emplace_back(obj.GetKey());
    }
  } else {
    std::cerr << "[ObjectStorageTransfor::List] Error: " << outcome.GetError().GetMessage() << "\n";
  }
  return ret;
}

absl::Status S3StorageProvider::Upload(const std::string &bucket, const std::string &object_key,
                                       const std::string &local_file_path) {
  if (!client_) {
    return absl::InternalError("[Upload] S3 client is not initialized");
  }

  std::ifstream input(local_file_path, std::ios::binary);
  if (!input.is_open()) {
    return absl::InternalError(fmt::format("[Upload] Failed to open local file: {}", local_file_path));
  }

  Aws::S3::Model::PutObjectRequest request;
  request.SetBucket(bucket);
  request.SetKey(object_key);

  auto stream =
      Aws::MakeShared<Aws::FStream>("UploadFileStream", local_file_path.c_str(), std::ios::in | std::ios::binary);
  request.SetBody(stream);

  auto outcome = client_->PutObject(request);
  if (!outcome.IsSuccess()) {
    return absl::InternalError(fmt::format("[Upload] Failed to upload: {}", outcome.GetError().GetMessage()));
  }
  return absl::OkStatus();
}
}  // namespace cppcommon::os
