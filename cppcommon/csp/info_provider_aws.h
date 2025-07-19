/**
 * @file info_provider_aws.h
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
class AWSInfoProvider : public CloudInfoProvider {
 public:
  std::optional<InstanceInfo> GetInstanceInfo() const override {
    InstanceInfo info;
    info.cloud_provider = CloudProvider::AWS;
    const std::string IMDS_HOST = "169.254.169.254";
    const std::string IMDS_BASE_PATH = "/latest/meta-data/";

    try {
      std::optional<std::string> instance_id_resp = HttpGet(IMDS_HOST, IMDS_BASE_PATH + "instance-id");
      if (!instance_id_resp) {
        LOG_ERR("[{}] {}", CloudProviderToString(info.cloud_provider),
                "Could not get instance-id. Not an AWS EC2 instance or network issue.");
        return std::nullopt;
      }
      info.instance_id = *instance_id_resp;

      info.instance_type = HttpGet(IMDS_HOST, IMDS_BASE_PATH + "instance-type").value_or("");
      std::string availability_zone = HttpGet(IMDS_HOST, IMDS_BASE_PATH + "placement/availability-zone").value_or("");
      info.zone = availability_zone;
      if (availability_zone.length() > 1) {
        info.region = availability_zone.substr(0, availability_zone.length() - 1);
      }
      info.private_ip = HttpGet(IMDS_HOST, IMDS_BASE_PATH + "local-ipv4").value_or("");
      info.public_ip = HttpGet(IMDS_HOST, IMDS_BASE_PATH + "public-ipv4").value_or("");
      return info;
    } catch (const std::exception& e) {
      LOG_ERR("[{}] {}", CloudProviderToString(info.cloud_provider), std::string("Exception: ") + e.what());
      return std::nullopt;
    }
  }
};
}  // namespace cppcommon
