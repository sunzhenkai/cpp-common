#include <cstdlib>
// self
#include "os.h"

namespace cppcommon {
std::string GetEnv(const std::string &key, const std::string &dft) {
  char *value = std::getenv(key.c_str());
  if (value == nullptr) {
    return dft;
  } else {
    return value;
  }
}
} // namespace cppcommon
