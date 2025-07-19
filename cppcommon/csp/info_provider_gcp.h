/**
 * @file info_provider_gcp.h
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
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

namespace cppcommon {
class GCPInfoProvider : public CloudInfoProvider {
 public:
  std::optional<InstanceInfo> GetInstanceInfo() const override {
    InstanceInfo info;
    info.cloud_provider = CloudProvider::GCP;
    const std::string METADATA_HOST = "metadata.google.internal";
    const std::string METADATA_BASE_PATH = "/computeMetadata/v1/";
    const httplib::Headers METADATA_HEADERS = {{"Metadata-Flavor", "Google"}};

    try {
      std::optional<std::string> instance_id_resp =
          HttpGet(METADATA_HOST, METADATA_BASE_PATH + "instance/id?alt=text", METADATA_HEADERS);
      if (!instance_id_resp) {
        // LOG_WARN("[{}] {}", CloudProviderToString(info.cloud_provider),
        //         "Could not get instance ID. Not a GCP Compute Engine instance or network issue.");
        return std::nullopt;
      }
      info.instance_id = *instance_id_resp;
      info.instance_name =
          HttpGet(METADATA_HOST, METADATA_BASE_PATH + "instance/name?alt=text", METADATA_HEADERS).value_or("");
      std::string machine_type_full =
          HttpGet(METADATA_HOST, METADATA_BASE_PATH + "instance/machine-type?alt=text", METADATA_HEADERS).value_or("");
      size_t last_slash = machine_type_full.rfind('/');
      if (last_slash != std::string::npos) {
        info.instance_type = machine_type_full.substr(last_slash + 1);
      } else {
        info.instance_type = machine_type_full;
      }

      std::string zone_full =
          HttpGet(METADATA_HOST, METADATA_BASE_PATH + "instance/zone?alt=text", METADATA_HEADERS).value_or("");
      last_slash = zone_full.rfind('/');
      if (last_slash != std::string::npos) {
        info.zone = zone_full.substr(last_slash + 1);
        size_t second_last_slash = zone_full.rfind('/', last_slash - 1);
        if (second_last_slash != std::string::npos) {
          info.region = zone_full.substr(second_last_slash + 1, last_slash - second_last_slash - 1);
        }
      } else {
        info.zone = zone_full;
      }

      std::optional<std::string> network_interfaces_json = HttpGet(
          METADATA_HOST, METADATA_BASE_PATH + "instance/network-interfaces/?recursive=true&alt=json", METADATA_HEADERS);
      if (network_interfaces_json) {
        rapidjson::Document doc;
        doc.Parse(network_interfaces_json->c_str());

        if (doc.HasParseError()) {
          LOG_ERR("[{}] {}", CloudProviderToString(info.cloud_provider),
                  std::string("RapidJSON parse error for network interfaces: ") +
                      rapidjson::GetParseError_En(doc.GetParseError()));
        } else if (doc.IsArray() && doc.Size() > 0) {
          const rapidjson::Value& first_interface = doc[0];
          if (first_interface.IsObject()) {
            if (first_interface.HasMember("ip") && first_interface["ip"].IsString()) {
              info.private_ip = first_interface["ip"].GetString();
            }

            if (first_interface.HasMember("accessConfigs") && first_interface["accessConfigs"].IsArray() &&
                first_interface["accessConfigs"].Size() > 0) {
              const rapidjson::Value& first_access_config = first_interface["accessConfigs"][0];
              if (first_access_config.IsObject() && first_access_config.HasMember("externalIp") &&
                  first_access_config["externalIp"].IsString()) {
                info.public_ip = first_access_config["externalIp"].GetString();
              }
            }
          }
        } else {
          LOG_ERR("[{}] {}", CloudProviderToString(info.cloud_provider),
                  "Network interfaces JSON is not a valid array or is empty.");
        }
      } else {
        LOG_ERR("[{}] {}", CloudProviderToString(info.cloud_provider), "Failed to get network interfaces JSON.");
      }
      return info;
    } catch (const std::exception& e) {
      LOG_ERR("[{}] {}", CloudProviderToString(info.cloud_provider), std::string("Exception: ") + e.what());
      return std::nullopt;
    }
  }
};
}  // namespace cppcommon
