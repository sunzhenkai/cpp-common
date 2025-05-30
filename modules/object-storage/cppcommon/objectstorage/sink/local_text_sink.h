
/**
 * @file base_sink_impl.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-05-30 16:20:22
 */
#include <filesystem>
#include <fstream>
#include <string>

#include "cppcommon/objectstorage/sink/base_sink.h"

namespace cppcommon {
template <typename Record>
class LocalSinkFileSystem : public SinkFileSystem<Record> {
 public:
  void Open(const std::string &filepath) override { ofs_.open(filepath, std::ios::out | std::ios::app); }
  bool IsOpen() override { return ofs_.is_open(); }

  void Close() override {
    if (ofs_) ofs_.close();
  }

  inline void Flush() override {
    if (ofs_) ofs_.flush();
  }

  static bool IsExists(const std::string &filepath) { return std::filesystem::exists(filepath); }

 protected:
  std::ofstream ofs_;
};

class LocalTextSinkFileSystem : public LocalSinkFileSystem<std::string> {
 public:
  inline int Write(std::string &&record) override {
    ofs_ << record << "\n";
    return 1;
  }
};

using LocalBasicSink = BaseSink<std::string, LocalTextSinkFileSystem>;
}  // namespace cppcommon
