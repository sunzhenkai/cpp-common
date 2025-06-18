#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "cppcommon/utils/common.h"
#include "cppcommon/utils/to_str.h"
#include "gtest/gtest.h"

struct CamparebleStruct {
  int value;
  bool operator<(const CamparebleStruct& other) const { return value < other.value; }
};

TEST(Common, TopnIndx) {
  std::vector<int> nums = {1, 3, 2, 4, 6, 5};
  auto idx = cppcommon::TopNIndex(nums, 100);
  std::cout << cppcommon::ToString(idx) << std::endl;

  idx = cppcommon::TopNIndex(nums, 1);
  ASSERT_EQ(idx.size(), 1);
  ASSERT_EQ(idx[0], 4);

  idx = cppcommon::TopNIndex(nums, 1, true);
  ASSERT_EQ(idx.size(), 1);
  ASSERT_EQ(idx[0], 0);

  std::cout << "---" << std::endl;
  std::vector<CamparebleStruct> ss = {{1}, {3}, {2}, {4}, {6}, {5}};

  idx = cppcommon::TopNIndex(nums, 100);
  std::cout << cppcommon::ToString(idx) << std::endl;

  idx = cppcommon::TopNIndex(nums, 1);
  ASSERT_EQ(idx.size(), 1);
  ASSERT_EQ(idx[0], 4);

  idx = cppcommon::TopNIndex(nums, 1, true);
  ASSERT_EQ(idx.size(), 1);
  ASSERT_EQ(idx[0], 0);
}

TEST(Common, GetFromMap) {
  std::unordered_map<std::string, std::string> m{
      {"a", "1"},
      {"b", "true"},
      {"c", "1.1"},
  };
  ASSERT_EQ(cppcommon::GetFromMap(m, "a", ""), "1");
  ASSERT_EQ(cppcommon::GetFromMap(m, "b", ""), "true");
  ASSERT_EQ(cppcommon::GetFromMap(m, "c", ""), "1.1");

  ASSERT_EQ(cppcommon::GetFromMap<int>(m, "a", 0), 1);
  ASSERT_EQ(cppcommon::GetFromMap<bool>(m, "b", false), true);
  ASSERT_EQ(cppcommon::GetFromMap<double>(m, "c", 0), 1.1);

  // bad cases
  ASSERT_EQ(cppcommon::GetFromMap<int>(m, "b", 0), 0);
  ASSERT_EQ(cppcommon::GetFromMap<bool>(m, "d", false), false);
  ASSERT_EQ(cppcommon::GetFromMap<bool>(m, "d", true), true);
  ASSERT_EQ(cppcommon::GetFromMap(m, "d", "non"), "non");
  std::cout << "> " << __func__ << std::endl;
}
