/**
 * @file instance_info.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-07-19 12:30:39
 */
#pragma once
#include <optional>

#include "cppcommon/csp/csp_structs.h"
namespace cppcommon {

class CloudInstanceInfoProvider {
 public:
  virtual std::optional<InstanceInfo> GetInstanceInfo() const = 0;
};

class CloudInfoProvider : public CloudInstanceInfoProvider {};

}  // namespace cppcommon
