#include <spdlog/spdlog.h>

#include <string>

#include "cppcommon/utils/compare.h"
#include "gtest/gtest.h"

TEST(Cmp, V) {
  std::string a = "c,a,b";
  std::string b = "d,a,c";

  auto r = cppcommon::Compare({.vector_options{.delimiter = ','}}, a, b);
  spdlog::info("{}", r.DiffString());
}
