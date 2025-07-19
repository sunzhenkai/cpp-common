#include "system_info.h"

#include <sys/sysinfo.h>
#include <unistd.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

namespace cppcommmon {
MachineInfo::MachineInfo() {
  InitStaticInfo();
  UpdateDynamicInfo();
}

// void GetOSInfo(MachineInfo::StaticInfo &info) {
//   static std::string os_release_file = "/etc/os-release";
//   std::ifstream os_release(os_release_file);
//   if (os_release.is_open()) {
//     std::string line;
//     while (std::getline(os_release, line)) {
//       if (line.find("PRETTY_NAME=") == 0) {
//         size_t start = line.find('"');
//         size_t end = line.rfind('"');
//         if (start != std::string::npos && end != std::string::npos && end > start) {
//           info.os_name = line.substr(start + 1, end - start - 1);
//         }
//       } else if (line.find("VERSION_ID=") == 0) {
//         size_t start = line.find('"');
//         size_t end = line.rfind('"');
//         if (start != std::string::npos && end != std::string::npos && end > start) {
//           info.os_version = line.substr(start + 1, end - start - 1);
//         } else {
//           info.os_version = line.substr(line.find('=') + 1);
//         }
//       }
//     }
//   } else {
//     std::cerr << __func__ << ": cannot open os release file " << os_release_file << std::endl;
//   }
//
//   static std::string proc_version_file = "/proc/version";
//   std::ifstream version(proc_version_file);
//   if (version.is_open()) {
//     std::string line;
//     if (version && std::getline(version, line)) {
//       info.kernel_version = line;
//     }
//   } else {
//     std::cerr << __func__ << ": cannot open proc version file " << proc_version_file << std::endl;
//   }
// }
//
// MachineInfo::StaticInfo GetStaticInfo() {
//   MachineInfo::StaticInfo info;
//   info.cpu_logical_cores = std::thread::hardware_concurrency();
//
//   // read cpu info
//   static const std::string proc_cpuinfo_file = "/proc/cpuinfo";
//   std::ifstream cpuinfo(proc_cpuinfo_file);
//   if (cpuinfo.is_open()) {
//     std::string line;
//     std::regex model_regex(R"(model name\s+:\s+(.+))");
//     std::set<std::string> core_ids;
//
//     while (std::getline(cpuinfo, line)) {
//       std::smatch match;
//       if (std::regex_search(line, match, model_regex)) {
//         info.cpu_model = match[1];
//       }
//
//       if (line.find("physical id") != std::string::npos || line.find("core id") != std::string::npos) {
//         core_ids.insert(line);
//       }
//     }
//     info.cpu_physical_cores = core_ids.empty() ? info.cpu_logical_cores / 2 : core_ids.size();
//   } else {
//     std::cerr << __func__ << ": cannot open proc cpuinfo file " << proc_cpuinfo_file << std::endl;
//   }
//
//   // read memory system_info
//   static const std::string proc_meminfo_file = "/proc/meminfo";
//   std::ifstream meminfo(proc_meminfo_file);
//   if (meminfo.is_open()) {
//     std::string line;
//     while (std::getline(meminfo, line)) {
//       if (line.find("MemTotal:") == 0) {
//         sscanf(line.c_str(), "MemTotal: %lu kB", &info.total_memory_kb);
//         break;
//       }
//     }
//   }
//
//   GetOSInfo(info);
//   return info;
// }
//
// void MachineInfo::InitStaticInfo() {}
}  // namespace cppcommmon
