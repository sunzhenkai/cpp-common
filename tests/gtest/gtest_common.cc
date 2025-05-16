#include <iostream>
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
