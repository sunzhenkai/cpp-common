#include <spdlog/spdlog.h>

#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "cppcommon/objectstorage/api.h"
#include "cppcommon/objectstorage/sink/base_sink.h"
#include "cppcommon/objectstorage/sink/local_csv_sink.h"
#include "cppcommon/utils/time.h"
#include "cppcommon/utils/time_ruler.h"
#include "gtest/gtest.h"

using namespace cppcommon;
using namespace cppcommon::os;
using std::chrono_literals::operator""s;

TEST(Sink, CsvBase) {
  CsvSink::Options options{.name = "runtime",
                           .name_options{.suffix = "csv"},
                           .roll_options{
                               .max_rows_per_file = 2,
                           },
                           .on_roll_callback = [](const std::string &fn, auto) { spdlog::info("rollfile: {}", fn); }};
  CsvSink s(std::move(options));
  s.Write(CsvRow{"a"});
  s.Write(CsvRow{"b"});
  s.Write(CsvRow{"c"});
  s.Write(CsvRow{","});
  s.Write(CsvRow{"\n"});
  s.Write(CsvRow{"\""});
}

TEST(Sink, CsvEsc) {
  CsvSink::Options options{.name = "runtime",
                           .name_options{.suffix = "csv"},
                           .on_roll_callback = [](const std::string &fn, auto) { spdlog::info("rollfile: {}", fn); }};
  CsvSink s(std::move(options));
  s.Write(CsvRow{"a"});
  s.Write(CsvRow{"b"});
  s.Write(CsvRow{"c"});
  s.Write(CsvRow{","});
  s.Write(CsvRow{"\n"});
  s.Write(CsvRow{"\""});
  s.Write(CsvRow{"\"\""});
  s.Write(CsvRow{"\r\n"});
}

TEST(Sink, CsvMt) {
  LocalBasicSink::Options options{
      .name = "runtime",
      .roll_options{
          .max_rows_per_file = 100 * 10000,
      },
      .on_roll_callback = [](const std::string &fn, auto) { spdlog::info("rollfile: {}", fn); }};
  LocalBasicSink s(std::move(options));

  std::atomic<int> counter(0);
  constexpr int kThreadCount = 12;
  constexpr int kWritesPerThread = 10000 * 1000;

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

  // std::this_thread::sleep_for(std::chrono::seconds(10));
}

TEST(Sink, CsvPmV2) {
  CsvSink::Options options{.name = "table",
                           .name_options{.suffix = "csv"},
                           .roll_options{.is_rotate = true, .max_rows_per_file = 10000 * 10},
                           .on_roll_callback = [](const std::string &fn, auto) { spdlog::info("rollfile: {}", fn); },
                           .ofs_options{.headers = {"a", "b2"}}};
  CsvSink s(std::move(options));
  auto record = CsvRow{"a2_1", "a2_2"};
  // auto record_b = CsvRow{"b2_1", "b2_2"};

  constexpr int kThreadCount = 8;
  constexpr int kWritesPerThread = 10000 * 5 + 10;

  auto writer = [&] {
    for (int i = 0; i < kWritesPerThread; ++i) {
      s.Write(record);
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

TEST(Sink, CsvDelim) {
  using DCsvSink = CsvSinkT<'|'>;

  DCsvSink ::Options options{.name = "table",
                             .name_options{.suffix = "csv"},
                             .roll_options{.is_rotate = true, .max_rows_per_file = 10000 * 10},
                             .on_roll_callback = [](const std::string &fn, auto) { spdlog::info("rollfile: {}", fn); },
                             .ofs_options{.headers = {"a", "b2"}}};
  DCsvSink s(std::move(options));
  auto record = CsvRow{"a2_1", "a2_2"};
  // auto record_b = CsvRow{"b2_1", "b2_2"};

  constexpr int kThreadCount = 8;
  constexpr int kWritesPerThread = 10000 * 5 + 10;

  auto writer = [&] {
    for (int i = 0; i < kWritesPerThread; ++i) {
      s.Write(record);
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

TEST(Sink, CsvBm) {
  cppcommon::TimeRuler tr;
  using DCsvSink = CsvSinkT<'|'>;

  DCsvSink ::Options options{
      .name = "table",
      .name_options{.suffix = "csv"},
      .roll_options{.is_rotate = true, .max_rows_per_file = 10000 * 10},
      .on_roll_callback = [](const std::string &fn, auto) { spdlog::info("rollfile: {}", fn); },
  };
  DCsvSink s(std::move(options));
  auto record = CsvRow{};
  for (auto i = 0; i < 500; ++i) {
    record.emplace_back(fmt::format("feature_value_{}", i));
  }

  constexpr int kThreadCount = 8;
  constexpr int kWritesPerThread = 5000 * 5 + 10;

  auto writer = [&] {
    for (int i = 0; i < kWritesPerThread; ++i) {
      s.Write(record);
    }
  };

  std::vector<std::thread> threads;
  for (int i = 0; i < kThreadCount; ++i) {
    threads.emplace_back(writer);
  }

  for (auto &t : threads) {
    t.join();
  }
  std::cout << "Elapsed: " << tr.ElapsedNow() << " ms\n";
}
