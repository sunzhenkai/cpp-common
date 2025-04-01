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

/**
 * @brief read file line by line
 * @param fn file path
 * @param cb call back
 * @param remove_cr remove the ending \r. when read the file in linux
 *  which generated in windows, the line text will endswith \r
 */
void ReadFileByLine(const char *fp, const std::function<void(size_t, std::string_view)> &cb, bool remove_cr = true);
}  // namespace cppcommon
