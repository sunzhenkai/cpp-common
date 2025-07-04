#include "storage_provider_s3.h"

#include <alibabacloud/oss/OssClient.h>
#include <alibabacloud/oss/auth/CredentialsProvider.h>
#include <aws/core/Aws.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/s3/model/ListBucketsRequest.h>
#include <aws/s3/model/ListObjectsRequest.h>
#include <aws/s3/model/ListObjectsV2Request.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <spdlog/spdlog.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "absl/status/status.h"
#include "cppcommon/extends/abseil/absl.h"
#include "cppcommon/objectstorage/transfor/storage_provider.h"
#include "cppcommon/utils/os.h"
#include "cppcommon/utils/str.h"
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

absl::StatusOr<FileList> S3StorageProvider::List(const std::string &bucket, const std::string &path) {
  Aws::S3::Model::ListObjectsRequest request;
  request.WithBucket(bucket).WithPrefix(path);  // .WithDelimiter("/");

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

absl::Status S3StorageProvider::Upload(const TransferMeta &m) {
  if (!client_) {
    return absl::InternalError("[Upload] S3 client is not initialized");
  }

  std::ifstream input(m.local_file_path, std::ios::binary);
  if (!input.is_open()) {
    return absl::InternalError(fmt::format("[Upload] Failed to open local file: {}", m.local_file_path));
  }

  Aws::S3::Model::PutObjectRequest request;
  request.SetBucket(m.bucket);
  request.SetKey(m.remote_file_path);

  auto stream =
      Aws::MakeShared<Aws::FStream>("UploadFileStream", m.local_file_path.c_str(), std::ios::in | std::ios::binary);
  request.SetBody(stream);

  auto outcome = client_->PutObject(request);
  if (!outcome.IsSuccess()) {
    return absl::InternalError(fmt::format("[Upload] Failed to upload: {}", outcome.GetError().GetMessage()));
  }
  return absl::OkStatus();
}

absl::Status S3StorageProvider::DownloadFile(const TransferMeta &m) {
  OkOrRet(PreDownloadFile(m));
  // download file
  auto rfp = TryRemoveCloudStoragePrefix(ServiceProvider::S3, m.bucket, m.remote_file_path);
  Aws::S3::Model::GetObjectRequest objectRequest;
  objectRequest.WithBucket(m.bucket).WithKey(rfp);
  auto outcome = client_->GetObject(objectRequest);
  // write file
  if (outcome.IsSuccess()) {
    std::ofstream localFile;
    localFile.open(m.local_file_path.c_str(), std::ios::out | std::ios::binary);
    if (localFile.is_open()) {
      const Aws::IOStream &s3Stream = outcome.GetResult().GetBody();
      localFile << s3Stream.rdbuf();
      localFile.close();
    } else {
      return absl::InternalError(FMT("cannot open file. [file={}]", m.local_file_path));
    }
  } else {
    return absl::InternalError(FMT("cannot download file from s3. [info={}, error={}, message={}]", m.ToString(),
                                   outcome.GetError().GetExceptionName(), outcome.GetError().GetMessage()));
  }
  return absl::OkStatus();
}

absl::StatusOr<FilePathList> S3StorageProvider::Download(const TransferMeta &meta) {
  ExpectOrInternal(client_, "client not inited");
  ExpectOrInternal(fs::is_directory(meta.local_file_path), "local file path must be directory");
  std::filesystem::path base = std::filesystem::path(meta.local_file_path);
  std::vector<std::filesystem::path> result;

  // List S3 objects
  Aws::S3::Model::ListObjectsV2Request request;
  request.SetBucket(meta.bucket);
  request.SetPrefix(TryRemoveCloudStoragePrefix(ServiceProvider::S3, meta.bucket, meta.remote_file_path));

  bool hasMoreObjects = true;
  while (hasMoreObjects) {
    auto outcome = client_->ListObjectsV2(request);
    ExpectOrInternal(outcome.IsSuccess(), FMT("list s3 object failed. [err={}]", outcome.GetError().GetMessage()));

    const auto &objectSummaries = outcome.GetResult().GetContents();
    for (const auto &obj : objectSummaries) {
      auto key = obj.GetKey();
      auto cur = GetObjLocalFilePath(meta.remote_file_path, meta.local_file_path, key);
      if (key.back() == '/') continue;
      auto dm = TransferMeta{.bucket = meta.bucket, .remote_file_path = key, .local_file_path = cur};
      auto s = DownloadFile(dm);
      if (!s.ok()) {
        spdlog::error("Download s3 object failed. [info={}, error={}]", dm.ToString(), s.ToString());
      }
      result.emplace_back(cur);
    }

    hasMoreObjects = outcome.GetResult().GetIsTruncated();
    if (hasMoreObjects) {
      request.SetContinuationToken(outcome.GetResult().GetNextContinuationToken());
    }
  }
  return result;
}
}  // namespace cppcommon::os
