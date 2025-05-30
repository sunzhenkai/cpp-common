#include <string>
#include <utility>

#include "cppcommon/objectstorage/sink/base_sink.h"
#include "gtest/gtest.h"

TEST(Sink, Base) {
  cppcommon::LocalBasicSink::Options options{.name = "runtime", .max_rows_per_file = 2};
  cppcommon::LocalBasicSink s(std::move(options));
  s.Write("a");
  s.Write("b");
  s.Write("c");
}
