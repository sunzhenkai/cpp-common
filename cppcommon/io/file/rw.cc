#include "rw.h"

#include <sys/resource.h>

#include <fstream>
#include <sstream>
#include <stdexcept>

#include "fstream"
#include "iostream"

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

void ReadFileByLine(const char *fn, const std::function<void(size_t, std::string_view)> &cb) {
  std::ifstream file(fn);
  if (file.is_open()) {
    std::string line;
    size_t line_number{0};
    while (std::getline(file, line)) {
      cb(line_number++, line);
    }
    file.close();
  } else {
    std::stringstream ss;
    ss << "open file failed. file=" << fn;
    throw std::runtime_error(ss.str());
  }
}

}  // namespace cppcommon
