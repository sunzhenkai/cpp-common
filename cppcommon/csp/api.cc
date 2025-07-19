#include "cppcommon/csp/api.h"

#include <chrono>
#include <future>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "cppcommon/csp/csp_structs.h"
#include "cppcommon/csp/info_provider_aliyun.h"
#include "cppcommon/csp/info_provider_aws.h"
#include "cppcommon/csp/info_provider_gcp.h"

namespace cppcommon {
CloudInfoFactory::CloudInfoFactory() { provider_ = CreateProvider(); }

std::unique_ptr<CloudInfoProvider> CloudInfoFactory::CreateProvider() {
  std::vector<std::unique_ptr<CloudInfoProvider>> providers;
  providers.push_back(std::make_unique<AliyunInfoProvider>());
  providers.push_back(std::make_unique<AWSInfoProvider>());
  providers.push_back(std::make_unique<GCPInfoProvider>());

  std::vector<std::future<std::optional<InstanceInfo>>> futures;
  for (size_t i = 0; i < providers.size(); ++i) {
    futures.push_back(
        std::async(std::launch::async, [provider = providers[i].get()]() { return provider->GetInstanceInfo(); }));
  }

  // wait for 3 seconds
  const auto total_timeout = std::chrono::seconds(3);
  auto start_time = std::chrono::steady_clock::now();

  while (std::chrono::steady_clock::now() - start_time < total_timeout) {
    for (size_t i = 0; i < futures.size(); ++i) {
      if (!futures[i].valid()) {
        continue;
      }
      if (futures[i].wait_for(std::chrono::milliseconds(10)) == std::future_status::ready) {
        std::optional<InstanceInfo> info = futures[i].get();
        if (info.has_value()) {
          std::cout << "Detected " << CloudProviderToString(info->cloud_provider) << " environment." << std::endl;
          return std::move(providers[i]);
        }
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  std::cerr << "Could not identify current cloud provider within timeout or not running on a supported cloud."
            << std::endl;
  return nullptr;
}

}  // namespace cppcommon
