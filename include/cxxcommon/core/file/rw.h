#pragma once

#include "vector"
#include "string"

namespace cxxcommon {
    std::vector<std::string> ReadLines(const char *fn);
    std::string ReadFile(const char *fn);
}