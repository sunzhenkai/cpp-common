/**
 * @file system_info.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-07-19 10:50:16
 */
#pragma once
#include <chrono>
#include <string>
#include <vector>

namespace cppcommmon {
struct MachineInfo {
  struct CpuStaticInfo {
    int cpu_logical_cores{0};
    int cpu_physical_cores{0};
    std::string cpu_model;
  };
  struct MemoryStaticInfo {
    uint64_t total_memory_kb{0};
  };
  struct OsInfo {
    std::string os_name;
    std::string os_version;
    std::string kernel_version;
  };
  struct CpuDynamicInfo {
    double load_avg_1min{0.0};
    double load_avg_5min{0.0};
    double load_avg_15min{0.0};
    // cpu util
    std::vector<double> cpu_util_percent;
    double cpu_util_percent_avg;
  };

  struct MemoryDynamicInfo {
    uint64_t mem_total_kb{0};
    uint64_t mem_available_kb{0};
    uint64_t mem_used_kb{0};
  };

  struct StaticInfo {
    CpuStaticInfo cpu;
    MemoryStaticInfo memory;
    OsInfo os;
  } static_info;

  struct DynamicInfo {
    CpuDynamicInfo cpu;
    MemoryDynamicInfo memory;
    std::chrono::steady_clock::time_point timestamp;
  } dynamic_info;

  MachineInfo();

  void InitStaticInfo();
  void UpdateDynamicInfo();
  std::string ToString() const;
  std::string ToJson() const;
};

std::string GetKernelVersionFromFile(const std::string& filepath);
void GetOsInfoFromOsRelease(MachineInfo::OsInfo& info);
void ParseCpuStaticInfo(MachineInfo::CpuStaticInfo& info);
void ParseMemoryStaticInfo(MachineInfo::MemoryStaticInfo& info);

void ParseLoadAvg(MachineInfo::CpuDynamicInfo& info);
void ParseCpuUtil(MachineInfo::CpuDynamicInfo& info);
void ParseMemoryDynamicInfo(MachineInfo::MemoryDynamicInfo& info);

void LoadOsInfo(MachineInfo::OsInfo& os_info);
}  // namespace cppcommmon
