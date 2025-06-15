#include <spdlog/spdlog.h>

#include <string>
#include <thread>
#include <utility>

#include "cppcommon/objectstorage/api.h"
#include "cppcommon/objectstorage/sink/base_sink.h"
#include "gtest/gtest.h"

using namespace cppcommon;
using namespace cppcommon::os;
using std::chrono_literals::operator""s;

TEST(Sink, Base) {
  LocalBasicSink::Options options{
      .name = "runtime",
      .roll_options{
          .max_rows_per_file = 2,
      },
      .on_roll_callback = [](const std::string &fn, auto) { spdlog::info("rollfile: {}", fn); }};
  LocalBasicSink s(std::move(options));
  s.Write("a");
  s.Write("b");
  s.Write("c");
}

TEST(Sink, BaseV2) {
  LocalBasicSink::Options options{
      .name = "runtime",
      .roll_options{
          .is_rotate = false,
      },
      .on_roll_callback = [](const std::string &fn, auto) { spdlog::info("rollfile: {}", fn); }};
  LocalBasicSink s(std::move(options));
  s.Write("a");
  s.Write("b");
  s.Write("c");
}

TEST(Sink, BaseV3) {
  LocalBasicSink::Options options{
      .name = "runtime",
      .roll_options{
          .is_rotate = true,
          .max_rows_per_file = 1,
          .max_backup_files = 3,
      },
      .on_roll_callback = [](const std::string &fn, auto) { spdlog::info("rollfile: {}", fn); }};
  LocalBasicSink s(std::move(options));
  s.Write("a");
  s.Write("b");
  s.Write("c");
  s.Write("d");
  s.Write("e");
  s.Write("f");
  s.Write("g");
  s.Write("h");
}

TEST(Sink, Long) {
  LocalBasicSink::Options options{
      .name = "runtime",
      .roll_options{.max_backup_files = 10,
                    .time_roll_policy{
                        .period = cppcommon::os::RollPeriod::MINITELY,
                        .path_fmt = cppcommon::os::TimeRollPathFormat::PARTED,
                    }},
      .on_roll_callback = [](const std::string &fn, const cppcommon::os::TimeRollPolicy &policy) {
        spdlog::info("rollfile: {}, {}", fn, policy.GetDatePath());
      }};
  LocalBasicSink s(std::move(options));

  for (auto i = 0; i < 60 * 3; ++i) {
    s.Write(std::to_string(i));
    std::this_thread::sleep_for(1s);
  }
}
