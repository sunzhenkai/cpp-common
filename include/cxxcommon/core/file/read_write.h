#pragma once

#include "string"
#include "vector"

namespace cxxcommon {
std::vector<std::string> ReadLines(const char* fn);
std::string ReadFile(const char* fn);
}  // namespace cxxcommon