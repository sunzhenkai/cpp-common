#include "str.h"

namespace cppcommon {
void StringSplit(std::vector<std::string> &result, const std::string &data, const char delimeter, bool ignore_empty) {
  result.clear();
  size_t start = 0;
  size_t end = data.find(delimeter);
  while (end != std::string::npos) {
    if (!ignore_empty || end > start) {
      result.emplace_back(data.substr(start, end - start));
    }
    start = end + 1;
    end = data.find(delimeter, start);
  }
  if (!ignore_empty || data.size() > start) {
    result.emplace_back(data.substr(start));
  }
}
}  // namespace cppcommon
