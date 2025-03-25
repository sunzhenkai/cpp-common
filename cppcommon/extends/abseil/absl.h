/**
 * @file absl.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-03-25 21:34:41
 */
#pragma once

namespace cppcommon::extends {
#define OkOrRet(expr) \
  if (auto value = expr; !value.ok()) return value;
#define ExpectOrRet(expr, status) \
  if (!(expr)) return status;
}  // namespace cppcommon::extends
