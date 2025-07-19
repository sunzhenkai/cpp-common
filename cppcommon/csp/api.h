/**
 * @file api.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-07-19 13:12:14
 */
#pragma once

#include <memory>

#include "cppcommon/csp/csp_structs.h"
#include "cppcommon/csp/info_provider_interface.h"
#include "cppcommon/partterns/singleton.h"

namespace cppcommon {
class CloudInfoFactory : public Singleton<CloudInfoFactory>, public CloudInfoProvider {
 public:
  CloudInfoFactory();
  static std::unique_ptr<CloudInfoProvider> CreateProvider();

  inline std::optional<InstanceInfo> GetInstanceInfo() const override {
    if (Instance().provider_) {
      return provider_->GetInstanceInfo();
    }
    return std::nullopt;
  }

 private:
  std::unique_ptr<CloudInfoProvider> provider_;
};
}  // namespace cppcommon
