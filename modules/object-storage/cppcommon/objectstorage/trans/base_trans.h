/**
 * @file base_trans.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-05-30 17:33:39
 */
#pragma once
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>

#include <memory>
#include <string>
#include <vector>

namespace cppcommon::os {
enum class StorageService {
  UNKNOWN,
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
    StorageService provider{StorageService::S3};
  };

  explicit ObjectStorageTransfor(StorageService provider);
  explicit ObjectStorageTransfor(Options &&options);

  static Options LoadFromEnv(StorageService provider);

  std::vector<std::string> List(const std::string &bucket, const std::string &path);

 private:
  std::shared_ptr<Aws::S3::S3Client> client_;
  Options options_;
};
}  // namespace cppcommon::os
