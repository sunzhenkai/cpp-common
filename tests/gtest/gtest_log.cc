#include "cppcommon/extends/spdlog/log.h"
#include "gtest/gtest.h"

TEST(Spdlog, Log) {
  RERROR("[{}] error.", __func__);
  RCRITICAL("[{}] critical.", __func__);

  // runtime
  RUNTIME_DEBUG("[{}] runtime debug.", __func__);
  RUNTIME_INFO("[{}] runtime info.", __func__);
  RUNTIME_WARN("[{}] runtime warn.", __func__);
  RUNTIME_ERROR("[{}] runtime error.", __func__);
}
