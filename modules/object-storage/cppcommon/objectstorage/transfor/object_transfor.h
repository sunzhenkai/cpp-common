/**
 * @file base_trans.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-05-30 17:33:39
 */
#pragma once
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include "cppcommon/objectstorage/transfor/storage_provider.h"
#include "cppcommon/objectstorage/transfor/storage_provider_gcs.h"
#include "cppcommon/objectstorage/transfor/storage_provider_oss.h"
#include "cppcommon/objectstorage/transfor/storage_provider_s3.h"

namespace cppcommon::os {
enum class ServiceProvider {
  OSS,  // aliyun
  S3,   // amazon
  GCS,  // gcp, google cloud storage
};

inline std::shared_ptr<StorageProvider> NewObjectTransfor(ServiceProvider provider) {
  switch (provider) {
    case ServiceProvider::OSS:
      return std::shared_ptr<StorageProvider>(new OssStorageProvider());
    case ServiceProvider::S3:
      return std::shared_ptr<StorageProvider>(new S3StorageProvider());
    case ServiceProvider::GCS:
      return std::shared_ptr<StorageProvider>(new GcsStorageProvider());
    default:
      throw std::runtime_error("unsupported storage service provider");
  }
}

inline std::shared_ptr<StorageProvider> NewObjectTransfor(ServiceProvider provider, StorageProviderOptions &&options) {
  switch (provider) {
    case ServiceProvider::OSS:
      return std::shared_ptr<StorageProvider>(new OssStorageProvider(std::move(options)));
    case ServiceProvider::S3:
      return std::shared_ptr<StorageProvider>(new S3StorageProvider(std::move(options)));
    default:
      throw std::runtime_error("unsupported storage service provider");
  }
}

inline std::shared_ptr<StorageProvider> NewObjectTransfor(ServiceProvider provider, const std::string &config_file) {
  switch (provider) {
    case ServiceProvider::GCS:
      return std::shared_ptr<StorageProvider>(new GcsStorageProvider(config_file));
    default:
      throw std::runtime_error("unsupported storage service provider");
  }
}
}  // namespace cppcommon::os
