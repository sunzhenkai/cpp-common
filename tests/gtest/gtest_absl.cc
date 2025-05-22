#include <spdlog/spdlog.h>

#include "absl/status/status.h"
#include "cppcommon/extends/abseil/absl.h"
#include "gtest/gtest.h"

TEST(Absl, Expect) {
  auto s = []() -> absl::Status {
    ExpectOrRetMsg(false, -1, "foo");
    return absl::OkStatus();
  };

  spdlog::info("{}", s().ToString());
}
