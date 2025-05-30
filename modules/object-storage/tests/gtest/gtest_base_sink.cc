#include <spdlog/spdlog.h>

#include <string>
#include <utility>

#include "cppcommon/objectstorage/api.h"
#include "gtest/gtest.h"

using namespace cppcommon;
using namespace cppcommon::os;

TEST(Sink, Base) {
  LocalBasicSink::Options options{.name = "runtime",
                                  .max_rows_per_file = 2,
                                  .on_roll_call_back = [](const std::string &fn) { spdlog::info("rollfile: {}", fn); }};
  LocalBasicSink s(std::move(options));
  s.Write("a");
  s.Write("b");
  s.Write("c");
}

TEST(Sink, BaseV2) {
  LocalBasicSink::Options options{.name = "runtime",
                                  .is_rotate = false,
                                  .on_roll_call_back = [](const std::string &fn) { spdlog::info("rollfile: {}", fn); }};
  LocalBasicSink s(std::move(options));
  s.Write("a");
  s.Write("b");
  s.Write("c");
}
