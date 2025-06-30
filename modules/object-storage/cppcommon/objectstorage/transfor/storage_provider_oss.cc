#include "storage_provider_oss.h"

#include <alibabacloud/oss/OssClient.h>
#include <alibabacloud/oss/auth/CredentialsProvider.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "absl/status/status.h"
#include "cppcommon/extends/abseil/absl.h"
#include "cppcommon/extends/fmt/fmt.h"
#include "cppcommon/objectstorage/transfor/storage_provider.h"
#include "cppcommon/utils/os.h"
#include "spdlog/spdlog.h"

namespace cppcommon::os {
namespace fs = std::filesystem;

std::string GetRegionFromEndpoint(const std::string &endpoint) {
  const std::string oss_prefix = "oss-";
  auto it = endpoint.find_first_of('.');
  if (it == std::string::npos || it <= oss_prefix.size()) {
    return "";
  } else {
    return endpoint.substr(oss_prefix.size(), it - oss_prefix.size());
  }
}

std::string GetOssRegion(const StorageProviderOptions &options) {
  if (options.region.empty()) {
    return GetRegionFromEndpoint(options.endpoint);
  } else {
    return options.region;
  }
}

StorageProviderOptions GetOssOptionsFromEnv() {
  StorageProviderOptions options;
  options.access_key_id = cppcommon::GetEnv("OSS_ACCESS_KEY_ID", "");
  options.access_key_secret = cppcommon::GetEnv("OSS_ACCESS_KEY_SECRET", "");
  options.endpoint = cppcommon::GetEnv("OSS_ENDPOINT", "");
  options.region = cppcommon::GetEnv("OSS_REGION", "");
  if (options.region.empty()) {
    options.region = GetRegionFromEndpoint(options.endpoint);
  }
  return options;
}

OssStorageProvider::OssStorageProvider(StorageProviderOptions &&options) {
  if (options.region.empty()) {
    options.region = GetRegionFromEndpoint(options.endpoint);
  }
  oss::ClientConfiguration conf;
  conf.signatureVersion = oss::SignatureVersionType::V4;
  auto credentialsProvider =
      std::make_shared<oss::SimpleCredentialsProvider>(options.access_key_id, options.access_key_secret);
  client_ = std::make_shared<oss::OssClient>(options.endpoint, credentialsProvider, conf);
  client_->SetRegion(GetOssRegion(options));
}

OssStorageProvider::OssStorageProvider() : OssStorageProvider(GetOssOptionsFromEnv()) {}

absl::StatusOr<FileList> OssStorageProvider::List(const std::string &bucket, const std::string &path) {
  std::vector<std::string> keys;
  oss::ListObjectsRequest request(bucket);
  request.setPrefix(path);
  // request.setDelimiter("/");

  auto outcome = client_->ListObjects(request);
  if (!outcome.isSuccess()) {
    spdlog::error("[OSS::List] Error: {}", outcome.error().Message());
    return keys;
  }

  for (const auto &obj : outcome.result().ObjectSummarys()) {
    keys.emplace_back(obj.Key());
  }
  return keys;
}

absl::Status OssStorageProvider::Upload(const TransferMeta &meta) {
  std::shared_ptr<std::iostream> fin =
      std::make_shared<std::fstream>(meta.local_file_path, std::ios::in | std::ios::binary);
  if (!fin->good()) {
    return absl::InternalError("cannot open file " + meta.local_file_path);
  }
  oss::PutObjectRequest request(meta.bucket, meta.remote_file_path, fin);
  auto outcome = client_->PutObject(request);
  if (!outcome.isSuccess()) {
    return absl::InternalError(
        absl::StrFormat("OSS Upload failed: code=%s, message=%s", outcome.error().Code(), outcome.error().Message()));
  }
  return absl::OkStatus();
}

absl::Status OssStorageProvider::DownloadFile(const TransferMeta &m) {
  OkOrRet(PreDownloadFile(m));
  auto parent = fs::path(m.local_file_path).parent_path();

  auto check_point_dir = parent.empty() ? fs::path("checkpoints") : fs::path(parent) / fs::path("checkpoints");
  fs::create_directories(check_point_dir.string());
  // download file
  auto rfp = TryRemoveCloudStoragePrefix(ServiceProvider::OSS, m.bucket, m.remote_file_path);
  oss::DownloadObjectRequest request(m.bucket, rfp, m.local_file_path, check_point_dir.string());
  auto outcome = client_->ResumableDownloadObject(request);
  ExpectOrInternal(outcome.isSuccess(),
                   FMT("download file from oss failed. [msg={}, host={}, request={}, dest_dir={}, check_point_dir={}]",
                       outcome.error().Message(), outcome.error().Host(), outcome.error().RequestId(),
                       m.local_file_path, check_point_dir.string()));
  fs::remove(check_point_dir);
  return absl::OkStatus();
}

absl::StatusOr<FilePathList> OssStorageProvider::Download(const TransferMeta &meta) {
  ExpectOrInternal(client_, "client not inited");
  ExpectOrInternal(fs::is_directory(meta.local_file_path), "local file path must be directory");
  std::vector<std::filesystem::path> result;

  // list files
  AlibabaCloud::OSS::ListObjectsV2Request list_request(meta.bucket);
  list_request.setPrefix(TryRemoveCloudStoragePrefix(ServiceProvider::OSS, meta.bucket, meta.remote_file_path));
  auto outcome = client_->ListObjectsV2(list_request);
  ExpectOrInternal(outcome.isSuccess(), FMT("list oss object failed. [err={}]", outcome.error().Message()));

  for (const auto &obj : outcome.result().ObjectSummarys()) {
    if (obj.Key().back() == '/') continue;
    auto cur = GetObjLocalFilePath(meta.remote_file_path, meta.local_file_path, obj.Key());
    auto dm = TransferMeta{.bucket = meta.bucket, .remote_file_path = obj.Key(), .local_file_path = cur};
    auto s = DownloadFile(dm);
    if (!s.ok()) {
      spdlog::error("Download oss object failed. [info={}, error={}]", dm.ToString(), s.ToString());
    }
    result.emplace_back(fs::path(cur));
  }
  return result;
}
}  // namespace cppcommon::os
