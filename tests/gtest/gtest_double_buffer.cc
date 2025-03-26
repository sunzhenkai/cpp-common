#include <unordered_map>

#include "cppcommon/partterns/double_buffer.h"
#include "gtest/gtest.h"

TEST(DoubleBuffer, S) {
  cppcommon::DoubleBuffer<std::unordered_map<std::string, std::string>> store;
  store.GetWrite().emplace("a", "1");
  // commit operation will trigger swap operation
  store.Commit([](auto &w) { w["b"] = "2"; });
  std::cout << store.GetRead().find("a")->second << std::endl;
  std::cout << store.GetRead().find("b")->second << std::endl;
}
