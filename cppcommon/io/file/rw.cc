#include "rw.h"

#include <sys/resource.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace cppcommon {
std::vector<std::string> ReadLines(const char *fn) {
  std::vector<std::string> result;
  std::ifstream file(fn);
  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line)) {
      result.emplace_back(line);
    }
    file.close();
  } else {
    std::cerr << "open file failed. file=" << fn << std::endl;
  }
  return result;
}

std::string ReadFile(const char *fn) {
  std::string result;
  std::ifstream infile(fn);
  infile.seekg(0, std::ios::end);
  size_t length = infile.tellg();
  infile.seekg(0, std::ios::beg);
  result.resize(length);
  infile.read(result.data(), length + 1);
  return result;
}

void ReadFileByLine(const char *fn, const std::function<void(size_t, std::string_view)> &cb, bool remove_cr) {
  std::ifstream file(fn);
  if (file.is_open()) {
    std::string line;
    size_t line_number{0};
    while (std::getline(file, line)) {
      if (remove_cr && line.size() > 0 && line[line.size() - 1] == '\r') {
        cb(line_number++, std::string_view(line.data(), line.size() - 1));
      } else {
        cb(line_number++, line);
      }
    }
    file.close();
  } else {
    std::stringstream ss;
    ss << "open file failed. file=" << fn;
    throw std::runtime_error(ss.str());
  }
}

}  // namespace cppcommon
