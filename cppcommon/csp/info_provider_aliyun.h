/**
 * @file info_provider_aliyun.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-07-19 12:41:56
 */
#pragma once
#include <string>

#include "cppcommon/csp/csp_structs.h"
#include "cppcommon/csp/info_provider_interface.h"
#include "cppcommon/extends/httplib/httplib_utils.h"
#include "cppcommon/extends/spdlog/log.h"

namespace cppcommon {
class AliyunInfoProvider : public CloudInfoProvider {
 public:
  std::optional<InstanceInfo> GetInstanceInfo() const override {
    InstanceInfo info;
    info.cloud_provider = CloudProvider::ALIYUN;
    const std::string IMS_HOST = "100.100.100.200";
    const std::string IMS_BASE_PATH = "/latest/meta-data/";

    try {
      std::optional<std::string> instance_id_resp = HttpGet(IMS_HOST, IMS_BASE_PATH + "instance-id");
      if (!instance_id_resp) {
        LOG_ERR("[{}] Could not get instance-id. Not an Aliyun ECS instance or network issue.",
                CloudProviderToString(info.cloud_provider));
        return std::nullopt;
      }
      info.instance_id = *instance_id_resp;
      info.instance_type = HttpGet(IMS_HOST, IMS_BASE_PATH + "instance/instance-type").value_or("");
      info.region = HttpGet(IMS_HOST, IMS_BASE_PATH + "region-id").value_or("");
      info.zone = HttpGet(IMS_HOST, IMS_BASE_PATH + "zone-id").value_or("");
      info.private_ip = HttpGet(IMS_HOST, IMS_BASE_PATH + "private-ipv4").value_or("");
      info.public_ip = HttpGet(IMS_HOST, IMS_BASE_PATH + "public-ipv4").value_or("");
      info.instance_name = "Aliyun Instance";
      return info;
    } catch (const std::exception& e) {
      LOG_ERR("[{}] {}", CloudProviderToString(info.cloud_provider), std::string("Exception: ") + e.what());
      return std::nullopt;
    }
  }
};
}  // namespace cppcommon
