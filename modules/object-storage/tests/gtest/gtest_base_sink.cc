#include <spdlog/spdlog.h>

#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "cppcommon/objectstorage/api.h"
#include "cppcommon/objectstorage/sink/base_sink.h"
#include "cppcommon/utils/time.h"
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
                        .period = cppcommon::os::RollPeriod::SECONDLY,
                        .path_fmt = cppcommon::os::TimeRollPathFormat::PARTED,
                    }},
      .on_roll_callback = [](const std::string &fn, const cppcommon::os::TimeRollPolicy &policy) {
        spdlog::info("rollfile: {}, pre={} - cur={}, {}", fn, policy.GetPreviousDatePath(), policy.GetDatePath(),
                     cppcommon::DateInfo(policy.last_rolling_ts_ms).Format("%H-%M-%S"));
      }};
  LocalBasicSink s(std::move(options));

  for (auto i = 0; i < 10 * 3; ++i) {
    auto msg = std::to_string(i);
    spdlog::info("<< {}", msg);
    s.Write(msg);
    std::this_thread::sleep_for(0.1s);
  }
}

TEST(Sink, Mt) {
  LocalBasicSink::Options options{
      .name = "runtime",
      .roll_options{
          .max_rows_per_file = 100 * 10000,
      },
      .on_roll_callback = [](const std::string &fn, auto) { spdlog::info("rollfile: {}", fn); }};
  LocalBasicSink s(std::move(options));

  std::atomic<int> counter(0);
  constexpr int kThreadCount = 8;
  constexpr int kWritesPerThread = 1000 * 10000;

  auto writer = [&] {
    for (int i = 0; i < kWritesPerThread; ++i) {
      int idx = counter.fetch_add(1, std::memory_order_relaxed);
      s.Write("data_" + std::to_string(idx));
    }
  };

  std::vector<std::thread> threads;
  for (int i = 0; i < kThreadCount; ++i) {
    threads.emplace_back(writer);
  }

  for (auto &t : threads) {
    t.join();
  }
}
