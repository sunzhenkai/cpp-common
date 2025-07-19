/**
 * @file csp_structs.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-07-19 12:37:40
 */
#pragma once
#include <string>

namespace cppcommon {
enum class CloudProvider { UNKNOWN, ALIYUN, AWS, GCP };

inline std::string CloudProviderToString(CloudProvider provider) {
  switch (provider) {
    case CloudProvider::ALIYUN:
      return "ALIYUN";
    case CloudProvider::AWS:
      return "AWS";
    case CloudProvider::GCP:
      return "GCP";
    case CloudProvider::UNKNOWN:
    default:
      return "UNKNOWN";
  }
}

struct InstanceInfo {
  CloudProvider cloud_provider = CloudProvider::UNKNOWN;
  std::string instance_id;
  std::string instance_name;
  std::string instance_type;
  std::string region;
  std::string zone;
  std::string private_ip;
  std::string public_ip;
  std::string os_type;

  inline std::string ToString() const {
    std::string s = "Cloud Provider: " + CloudProviderToString(cloud_provider) + "\n";
    s += "Instance ID: " + instance_id + "\n";
    s += "Instance Name: " + instance_name + "\n";
    s += "Instance Type: " + instance_type + "\n";
    s += "Region: " + region + "\n";
    s += "Zone: " + zone + "\n";
    s += "Private IP: " + private_ip + "\n";
    s += "Public IP: " + (public_ip.empty() ? "N/A" : public_ip) + "\n";
    s += "OS Type: " + os_type + "\n";
    return s;
  }
};

}  // namespace cppcommon
