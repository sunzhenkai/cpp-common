#include <iostream>
#include <vector>

#include "cppcommon/utils/common.h"
#include "cppcommon/utils/to_str.h"
#include "gtest/gtest.h"

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
}
