#include "file/rw.h"
#include "fstream"
#include "iostream"

namespace cpp_common {
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
} // namespace cpp_common
