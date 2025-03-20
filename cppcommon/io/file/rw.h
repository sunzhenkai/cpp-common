/**
 * @file rw.h
 * @brief read & write file functions
 */

#pragma once

#include <fstream>
#include <functional>
#include <sstream>
#include <string_view>

#include "string"
#include "vector"

namespace cppcommon {
std::vector<std::string> ReadLines(const char *fn);
std::string ReadFile(const char *fn);

template <typename T>
void WriteFile(const char *fp, const T &data) {
  std::ofstream out(fp);
  if (out.is_open()) {
    out << data;
    out.close();
  } else {
    std::stringstream ss;
    ss << "open file failed. file=" << fp;
    throw std::runtime_error(ss.str());
  }
}

void ReadFileByLine(const char *fn, const std::function<void(size_t, std::string_view)> &cb);
}  // namespace cppcommon
