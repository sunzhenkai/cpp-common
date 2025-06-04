#include "storage_provider_oss.h"

#include <alibabacloud/oss/OssClient.h>
#include <alibabacloud/oss/auth/CredentialsProvider.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "absl/status/status.h"
#include "cppcommon/extends/abseil/absl.h"
#include "cppcommon/extends/fmt/fmt.h"
#include "cppcommon/objectstorage/transfor/storage_provider.h"
#include "cppcommon/utils/error.h"
#include "cppcommon/utils/os.h"
#include "cppcommon/utils/str.h"
#include "spdlog/spdlog.h"

namespace cppcommon::os {
namespace fs = std::filesystem;

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

absl::StatusOr<FileList> OssStorageProvider::List(const std::string &bucket, const std::string &path) {
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

absl::Status OssStorageProvider::Upload(const TransferMeta &meta) {
  auto outcome = client_->PutObject(meta.bucket, meta.remote_file_path, meta.local_file_path);
  if (!outcome.isSuccess()) {
    return absl::InternalError(
        absl::StrFormat("OSS Upload failed: code=%s, message=%s", outcome.error().Code(), outcome.error().Message()));
  }
  return absl::OkStatus();
}

absl::Status OssStorageProvider::DownloadFile(const TransferMeta &m) {
  auto &lp = m.local_file_path;
  ExpectOrInternal(!lp.empty(), FMT("destination path should not be empty. [{}]", m.ToString()));
  ExpectOrInternal(lp[lp.size() - 1] != '/', FMT("destination file path should not end with '/'. [{}]", m.ToString()));
  // 1. ensure local dest path
  OkOrRet(EnsureLocalPath(fs::path(m.local_file_path), m.overwrite));
  // 2. download file
  auto check_point_dir = fs::path(lp).parent_path() / fs::path("checkpoints");
  fs::create_directories(check_point_dir.string());
  oss::DownloadObjectRequest request(m.bucket, m.remote_file_path, m.local_file_path, check_point_dir.string());
  auto outcome = client_->ResumableDownloadObject(request);
  ExpectOrInternal(outcome.isSuccess(),
                   FMT("download file from oss failed. [msg={}, host={}, request={}, dest_dir={}, check_point_dir={}]",
                       outcome.error().Message(), outcome.error().Host(), outcome.error().RequestId(),
                       m.local_file_path, check_point_dir.string()));
  return absl::OkStatus();
}

absl::StatusOr<FilePathList> OssStorageProvider::Download(const TransferMeta &meta) {
  Expect(client_, "oss client not inited");
  std::filesystem::path base = std::filesystem::path(meta.local_file_path) / std::filesystem::path(meta.file_name);
  std::vector<std::filesystem::path> result;
  // list files
  AlibabaCloud::OSS::ListObjectsV2Request list_request(meta.bucket);
  list_request.setPrefix(meta.remote_file_path);
  auto outcome = client_->ListObjectsV2(list_request);
  Expect(outcome.isSuccess(), FMT("list oss object failed. [err={}]", outcome.error().Message()));
  for (const auto &obj : outcome.result().ObjectSummarys()) {
    auto cur = base / std::filesystem::path(obj.Key().substr(meta.remote_file_path.size()));
    if (cppcommon::EndsWith(obj.Key(), "/")) continue;
    auto dm = TransferMeta{.bucket = meta.bucket, .remote_file_path = obj.Key(), .local_file_path = cur};
    auto s = DownloadFile(dm);
    if (!s.ok()) {
      spdlog::error("Download oss object failed. [info={}, error={}]", dm.ToString(), s.ToString());
    } else {
      spdlog::info("Download oss object success. [info={}]", dm.ToString());
    }
    result.emplace_back(cur);
  }
  return result;
}
}  // namespace cppcommon::os
