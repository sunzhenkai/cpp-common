#include "base_trans.h"

#include <aws/core/Aws.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/ListBucketsRequest.h>
#include <aws/s3/model/ListObjectsRequest.h>

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "cppcommon/utils/os.h"

namespace cppcommon::os {
ObjectStorageTransfor::Options ObjectStorageTransfor::LoadFromEnv(StorageService provider) {
  ObjectStorageTransfor::Options options;
  options.provider = provider;
  if (provider == StorageService::S3) {
    options.access_key_id = cppcommon::GetEnv("AWS_ACCESS_KEY_ID", "");
    options.access_key_secret = cppcommon::GetEnv("AWS_SECRET_ACCESS_KEY", "");
    options.region = cppcommon::GetEnv("AWS_REGION", "");
    options.endpoint = cppcommon::GetEnv("AWS_ENDPOINT", "");
  } else if (provider == StorageService::OSS) {
    options.access_key_id = cppcommon::GetEnv("OSS_ACCESS_KEY_ID", "");
    options.access_key_secret = cppcommon::GetEnv("OSS_ACCESS_KEY_SECRET", "");
    options.region = cppcommon::GetEnv("OSS_REGION", "");
    options.endpoint = cppcommon::GetEnv("OSS_ENDPOINT", "");
  } else if (provider == StorageService::GCS) {
    options.access_key_id = cppcommon::GetEnv("GCS_ACCESS_KEY_ID", "");
    options.access_key_secret = cppcommon::GetEnv("GCS_ACCESS_KEY_SECRET", "");
    options.region = cppcommon::GetEnv("GCS_REGION", "");
    options.endpoint = cppcommon::GetEnv("GCS_ENDPOINT", "");
  }
  return options;
}

ObjectStorageTransfor::ObjectStorageTransfor(StorageService provider) : ObjectStorageTransfor(LoadFromEnv(provider)) {}

ObjectStorageTransfor::ObjectStorageTransfor(Options &&options) : options_(std::move(options)) {
  Aws::Client::ClientConfiguration config;
  if (!options_.endpoint.empty()) {
    config.endpointOverride = options_.endpoint;
  }
  if (!options_.region.empty()) {
    config.region = options_.region;
  }
  if (options_.provider == StorageService::OSS) {
    config.scheme = Aws::Http::Scheme::HTTP;
  }

  Aws::Auth::AWSCredentials credentials(options_.access_key_id, options_.access_key_secret);
  client_ = std::make_shared<Aws::S3::S3Client>(credentials, nullptr, config);
}

std::vector<std::string> ObjectStorageTransfor::List(const std::string &bucket, const std::string &path) {
  Aws::S3::Model::ListObjectsRequest request;
  request.WithBucket(bucket);

  std::vector<std::string> ret;
  auto outcome = client_->ListObjects(request);
  if (outcome.IsSuccess()) {
    std::cout << "Objects in bucket:\n";
    for (const auto &obj : outcome.GetResult().GetContents()) {
      ret.emplace_back(obj.GetKey());
    }
  } else {
    std::cerr << "Error: " << outcome.GetError().GetMessage() << "\n";
  }
  return ret;
}
}  // namespace cppcommon::os
