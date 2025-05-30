/**
 * @file base_trans.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-05-30 17:33:39
 */
#pragma once
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>

#include <string>
#include <utility>

namespace cppcommon::os {
enum class StorageService {
  OSS,  // aliyun
  S3,   // amazon
  GCS,  // gcp, google cloud storage
};

class ObjectStorageTransfor {
 public:
  struct Options {
    std::string access_key_id;
    std::string access_key_secret;
    std::string region;
    std::string endpoint;
    std::string bucket;
  };

  explicit ObjectStorageTransfor(Options &&options) : options_(std::move(options)) {}

 private:
  Aws::S3::S3Client client_;
  Options options_;
};
}  // namespace cppcommon::os
