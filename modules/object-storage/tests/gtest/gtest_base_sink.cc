#include <spdlog/spdlog.h>

#include <string>
#include <utility>

#include "cppcommon/objectstorage/api.h"
#include "gtest/gtest.h"

TEST(Sink, Base) {
  cppcommon::LocalBasicSink::Options options{
      .name = "runtime", .max_rows_per_file = 2, .on_roll_call_back = [](const std::string &fn) {
        spdlog::info("rollfile: {}", fn);
      }};
  cppcommon::LocalBasicSink s(std::move(options));
  s.Write("a");
  s.Write("b");
  s.Write("c");
}

TEST(Sink, BaseV2) {
  cppcommon::LocalBasicSink::Options options{
      .name = "runtime", .is_rotate = false, .on_roll_call_back = [](const std::string &fn) {
        spdlog::info("rollfile: {}", fn);
      }};
  cppcommon::LocalBasicSink s(std::move(options));
  s.Write("a");
  s.Write("b");
  s.Write("c");
}
