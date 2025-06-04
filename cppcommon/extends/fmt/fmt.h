/**
 * @file fmt.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-06-04 22:33:53
 */
#pragma once
#include "fmt/format.h"

#ifndef FMT
#define FMT(...) fmt::format(__VA_ARGS__)
#endif
