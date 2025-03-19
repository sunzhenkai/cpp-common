/**
 * @file rw.h
 * @brief read & write file functions
 */

#pragma once

#include "string"
#include "vector"

namespace cppcommon {
std::vector<std::string> ReadLines(const char *fn);
std::string ReadFile(const char *fn);
} // namespace cppcommon
