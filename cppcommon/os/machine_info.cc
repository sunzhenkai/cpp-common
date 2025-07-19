#include "machine_info.h"

#include <sys/sysinfo.h>
#include <unistd.h>

#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

namespace cppcommmon {
std::string GetKernelVersionFromFile(const std::string& filepath) {
  std::string kernel_version;
  std::ifstream file(filepath);
  if (file.is_open()) {
    std::string line;
    if (std::getline(file, line)) {
      kernel_version = line;
    }
  } else {
    std::cerr << "Error: Cannot open " << filepath << std::endl;
  }
  return kernel_version;
}

void GetOsInfoFromOsRelease(MachineInfo::OsInfo& info) {
  std::ifstream file("/etc/os-release");
  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line)) {
      if (line.rfind("NAME=", 0) == 0) {  // rfind with 0 means starts with
        info.os_name = line.substr(5);
        if (info.os_name.length() > 2 && info.os_name.front() == '"' && info.os_name.back() == '"') {
          info.os_name = info.os_name.substr(1, info.os_name.length() - 2);
        }
      } else if (line.rfind("VERSION=", 0) == 0) {
        info.os_version = line.substr(8);
        if (info.os_version.length() > 2 && info.os_version.front() == '"' && info.os_version.back() == '"') {
          info.os_version = info.os_version.substr(1, info.os_version.length() - 2);
        }
      }
    }
  } else {
    std::cerr << "Error: Cannot open /etc/os-release" << std::endl;
  }
}

void ParseCpuStaticInfo(MachineInfo::CpuStaticInfo& info) {
  std::ifstream file("/proc/cpuinfo");
  if (file.is_open()) {
    std::string line;
    std::string current_model_name;
    while (std::getline(file, line)) {
      if (line.rfind("processor", 0) == 0) {
        info.cpu_logical_cores++;
      } else if (line.rfind("cpu cores", 0) == 0) {
        std::stringstream ss(line);
        std::string key;
        std::string value_str;
        ss >> key >> key >> key >> value_str;  // "cpu cores : N"
        try {
          info.cpu_physical_cores = std::stoi(value_str);
        } catch (const std::exception& e) {
          std::cerr << "Warning: Failed to parse cpu_physical_cores: " << e.what() << ", value: " << value_str
                    << std::endl;
        }
      } else if (line.rfind("model name", 0) == 0) {
        std::stringstream ss(line);
        std::string key;
        ss >> key >> key;                                                   // "model name :"
        std::getline(ss, info.cpu_model);                                   // Read the rest of the line
        info.cpu_model.erase(0, info.cpu_model.find_first_not_of(" \t:"));  // Trim leading spaces/colon
      }
    }
    // Fallback for physical cores if "cpu cores" isn't present for some reason
    if (info.cpu_physical_cores == 0 && info.cpu_logical_cores > 0) {
      // A common heuristic is that physical cores are often logical cores / 2 for hyperthreading
      // Or we can count unique physical_id entries if available, but it's more complex.
      // For simplicity, we'll assume a single physical package if cpu_physical_cores wasn't found.
      info.cpu_physical_cores = 1;  // Default to at least one physical core
    }
  } else {
    std::cerr << "Error: Cannot open /proc/cpuinfo" << std::endl;
  }
}

void ParseMemoryStaticInfo(MachineInfo::MemoryStaticInfo& info) {
  std::ifstream file("/proc/meminfo");
  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line)) {
      if (line.rfind("MemTotal:", 0) == 0) {
        std::stringstream ss(line);
        std::string key;
        std::string value_str;
        std::string unit;
        ss >> key >> value_str >> unit;  // "MemTotal: NNNN kB"
        try {
          info.total_memory_kb = std::stoull(value_str);
        } catch (const std::exception& e) {
          std::cerr << "Warning: Failed to parse total_memory_kb: " << e.what() << std::endl;
        }
        break;  // Found MemTotal, no need to read further
      }
    }
  } else {
    std::cerr << "Error: Cannot open /proc/meminfo" << std::endl;
  }
}

void ParseLoadAvg(MachineInfo::CpuDynamicInfo& info) {
  std::ifstream file("/proc/loadavg");
  if (file.is_open()) {
    std::string line;
    if (std::getline(file, line)) {
      std::stringstream ss(line);
      ss >> info.load_avg_1min >> info.load_avg_5min >> info.load_avg_15min;
    }
  } else {
    std::cerr << "Error: Cannot open /proc/loadavg" << std::endl;
  }
}

void ParseCpuUtil(MachineInfo::CpuDynamicInfo& info) {
  std::ifstream file("/proc/stat");
  if (file.is_open()) {
    std::string line;
    info.cpu_util_percent.clear();  // Clear previous data

    // Read first line for overall CPU
    if (std::getline(file, line)) {
      if (line.rfind("cpu ", 0) == 0) {
        std::stringstream ss(line);
        std::string cpu_label;
        int64_t user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
        ss >> cpu_label >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;

        int64_t total_cpu_time = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
        if (total_cpu_time > 0) {
          double idle_time_percent = (static_cast<double>(idle) / total_cpu_time) * 100.0;
          info.cpu_util_percent_avg = 100.0 - idle_time_percent;
        }
      }
    }

    // Read subsequent lines for individual CPU cores (cpu0, cpu1, ...)
    while (std::getline(file, line)) {
      if (line.rfind("cpu", 0) == 0 && line.length() > 3 && isdigit(line[3])) {  // "cpuN"
        std::stringstream ss(line);
        std::string cpu_label;
        int64_t user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
        ss >> cpu_label >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;

        int64_t total_cpu_time = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
        if (total_cpu_time > 0) {
          double idle_time_percent = (static_cast<double>(idle) / total_cpu_time) * 100.0;
          info.cpu_util_percent.push_back(100.0 - idle_time_percent);
        }
      }
    }
  } else {
    std::cerr << "Error: Cannot open /proc/stat" << std::endl;
  }
}

void ParseMemoryDynamicInfo(MachineInfo::MemoryDynamicInfo& info) {
  std::ifstream file("/proc/meminfo");
  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line)) {
      if (line.rfind("MemTotal:", 0) == 0) {
        std::stringstream ss(line);
        std::string key;
        std::string value_str;
        std::string unit;
        ss >> key >> value_str >> unit;
        try {
          info.mem_total_kb = std::stoull(value_str);
        } catch (const std::exception& e) {
          std::cerr << "Warning: Failed to parse mem_total_kb: " << e.what() << std::endl;
        }
      } else if (line.rfind("MemAvailable:", 0) == 0) {
        std::stringstream ss(line);
        std::string key;
        std::string value_str;
        std::string unit;
        ss >> key >> value_str >> unit;
        try {
          info.mem_available_kb = std::stoull(value_str);
        } catch (const std::exception& e) {
          std::cerr << "Warning: Failed to parse mem_available_kb: " << e.what() << std::endl;
        }
      }
      // If both are found, we can break early for efficiency
      if (info.mem_total_kb > 0 && info.mem_available_kb > 0) {
        info.mem_used_kb = info.mem_total_kb - info.mem_available_kb;
        break;
      }
    }
  } else {
    std::cerr << "Error: Cannot open /proc/meminfo" << std::endl;
  }
}

MachineInfo::MachineInfo() {
  InitStaticInfo();
  UpdateDynamicInfo();
}

void MachineInfo::InitStaticInfo() {
  ParseCpuStaticInfo(static_info.cpu);
  ParseMemoryStaticInfo(static_info.memory);
  LoadOsInfo(static_info.os);
}

void MachineInfo::UpdateDynamicInfo() {
  ParseLoadAvg(dynamic_info.cpu);
  ParseCpuUtil(dynamic_info.cpu);
  ParseMemoryDynamicInfo(dynamic_info.memory);
  dynamic_info.timestamp = std::chrono::steady_clock::now();
}

void LoadOsInfo(MachineInfo::OsInfo& os_info) {
  GetOsInfoFromOsRelease(os_info);
  os_info.kernel_version = GetKernelVersionFromFile("/proc/version");
}

std::string MachineInfo::ToString() const {
  std::ostringstream oss;
  oss << "--- Machine Information ---\n";

  oss << "\n--- Static Info ---\n";
  oss << "  CPU Logical Cores: " << static_info.cpu.cpu_logical_cores << "\n";
  oss << "  CPU Physical Cores: " << static_info.cpu.cpu_physical_cores << "\n";
  oss << "  CPU Model: " << static_info.cpu.cpu_model << "\n";
  oss << "  Total Memory (KB): " << static_info.memory.total_memory_kb << "\n";
  oss << "  OS Name: " << static_info.os.os_name << "\n";
  oss << "  OS Version: " << static_info.os.os_version << "\n";
  oss << "  Kernel Version: " << static_info.os.kernel_version << "\n";

  oss << "\n--- Dynamic Info ---\n";
  oss << "  Load Average (1min): " << std::fixed << std::setprecision(2) << dynamic_info.cpu.load_avg_1min << "\n";
  oss << "  Load Average (5min): " << std::fixed << std::setprecision(2) << dynamic_info.cpu.load_avg_5min << "\n";
  oss << "  Load Average (15min): " << std::fixed << std::setprecision(2) << dynamic_info.cpu.load_avg_15min << "\n";
  oss << "  CPU Utilization Avg (Overall): " << std::fixed << std::setprecision(2)
      << dynamic_info.cpu.cpu_util_percent_avg << "%\n";
  oss << "  CPU Utilization (Individual Cores):\n";
  for (size_t i = 0; i < dynamic_info.cpu.cpu_util_percent.size(); ++i) {
    oss << "    CPU" << (i - 1) << ": " << std::fixed << std::setprecision(2) << dynamic_info.cpu.cpu_util_percent[i]
        << "%\n";
  }
  oss << "  Memory Total (KB): " << dynamic_info.memory.mem_total_kb << "\n";
  oss << "  Memory Available (KB): " << dynamic_info.memory.mem_available_kb << "\n";
  oss << "  Memory Used (KB): " << dynamic_info.memory.mem_used_kb << "\n";
  return oss.str();
}

std::string MachineInfo::ToJson() const {
  rapidjson::Document document;
  document.SetObject();  // 根是一个 JSON 对象

  rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

  // Static Info
  rapidjson::Value static_info_obj(rapidjson::kObjectType);

  rapidjson::Value cpu_static_obj(rapidjson::kObjectType);
  cpu_static_obj.AddMember("cpu_logical_cores", static_info.cpu.cpu_logical_cores, allocator);
  cpu_static_obj.AddMember("cpu_physical_cores", static_info.cpu.cpu_physical_cores, allocator);
  cpu_static_obj.AddMember("cpu_model", rapidjson::Value(static_info.cpu.cpu_model.c_str(), allocator).Move(),
                           allocator);
  static_info_obj.AddMember("cpu", cpu_static_obj, allocator);

  rapidjson::Value memory_static_obj(rapidjson::kObjectType);
  memory_static_obj.AddMember("total_memory_kb", static_info.memory.total_memory_kb, allocator);
  static_info_obj.AddMember("memory", memory_static_obj, allocator);

  rapidjson::Value os_info_obj(rapidjson::kObjectType);
  os_info_obj.AddMember("os_name", rapidjson::Value(static_info.os.os_name.c_str(), allocator).Move(), allocator);
  os_info_obj.AddMember("os_version", rapidjson::Value(static_info.os.os_version.c_str(), allocator).Move(), allocator);
  os_info_obj.AddMember("kernel_version", rapidjson::Value(static_info.os.kernel_version.c_str(), allocator).Move(),
                        allocator);
  static_info_obj.AddMember("os", os_info_obj, allocator);

  document.AddMember("static_info", static_info_obj, allocator);

  // Dynamic Info
  rapidjson::Value dynamic_info_obj(rapidjson::kObjectType);

  rapidjson::Value cpu_dynamic_obj(rapidjson::kObjectType);
  cpu_dynamic_obj.AddMember("load_avg_1min", dynamic_info.cpu.load_avg_1min, allocator);
  cpu_dynamic_obj.AddMember("load_avg_5min", dynamic_info.cpu.load_avg_5min, allocator);
  cpu_dynamic_obj.AddMember("load_avg_15min", dynamic_info.cpu.load_avg_15min, allocator);
  cpu_dynamic_obj.AddMember("cpu_util_percent_avg", dynamic_info.cpu.cpu_util_percent_avg, allocator);

  rapidjson::Value cpu_util_array(rapidjson::kArrayType);
  for (double util : dynamic_info.cpu.cpu_util_percent) {
    cpu_util_array.PushBack(util, allocator);
  }
  cpu_dynamic_obj.AddMember("cpu_util_percent", cpu_util_array, allocator);
  dynamic_info_obj.AddMember("cpu", cpu_dynamic_obj, allocator);

  rapidjson::Value memory_dynamic_obj(rapidjson::kObjectType);
  memory_dynamic_obj.AddMember("mem_total_kb", dynamic_info.memory.mem_total_kb, allocator);
  memory_dynamic_obj.AddMember("mem_available_kb", dynamic_info.memory.mem_available_kb, allocator);
  memory_dynamic_obj.AddMember("mem_used_kb", dynamic_info.memory.mem_used_kb, allocator);
  dynamic_info_obj.AddMember("memory", memory_dynamic_obj, allocator);

  int64_t timestamp_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(dynamic_info.timestamp.time_since_epoch()).count();
  dynamic_info_obj.AddMember("timestamp_ms", timestamp_ms, allocator);

  document.AddMember("dynamic_info", dynamic_info_obj, allocator);

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  document.Accept(writer);

  return buffer.GetString();
}
}  // namespace cppcommmon
