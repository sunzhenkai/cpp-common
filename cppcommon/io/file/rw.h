/**
 * @file rw.h
 * @brief read & write file functions
 */

#pragma once

#include <functional>
#include <string_view>

#include "string"
#include "vector"

namespace cppcommon {
std::vector<std::string> ReadLines(const char *fn);
std::string ReadFile(const char *fn);
void ReadFileByLine(const char *fn, const std::function<void(size_t, std::string_view)> &cb);
}  // namespace cppcommon
