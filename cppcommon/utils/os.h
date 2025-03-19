/**
 * @file os.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-03-19 15:09:01
 */
#pragma once
#include <string>

namespace cppcommon {
/**
 * @brief Get envrionment variable with default value.
 * @param [in] key envrionment variable key
 * @param [in] dft default value
 */
std::string GetEnv(const std::string &key, const std::string &dft);
} // namespace cppcommon
