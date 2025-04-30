#include <array>
#include <deque>
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <string>

#include "cppcommon/utils/type_traits.h"
#include "gtest/gtest.h"

TEST(TypeTraits, STL) {
  ASSERT_TRUE(cppcommon::is_vector<std::vector<int>>::value);
  ASSERT_TRUE(cppcommon::is_vector_v<std::vector<int>>);
  ASSERT_TRUE(cppcommon::is_vector_v<const std::vector<int> &>);
  auto v = cppcommon::is_array<std::array<int, 5>>::value;
  ASSERT_TRUE(v);
  v = cppcommon::is_array<const std::array<int, 5> &>::value;
  ASSERT_TRUE(v);
  v = cppcommon::is_array_v<std::array<int, 5>>;
  ASSERT_TRUE(v);
  v = cppcommon::is_array_v<const std::array<int, 5> &>;
  ASSERT_TRUE(v);
  v = cppcommon::is_map_v<std::map<int, int>>;
  ASSERT_TRUE(v);
  v = cppcommon::is_map_v<std::multimap<int, int>>;
  ASSERT_TRUE(v);
  v = cppcommon::is_map_v<const std::multimap<int, int> &>;
  ASSERT_TRUE(v);
  v = cppcommon::is_map<const std::multimap<int, int> &>::value;
  ASSERT_TRUE(v);
  ASSERT_TRUE(cppcommon::is_container_v<std::vector<int>>);
  v = cppcommon::is_container_v<std::array<int, 1>>;
  ASSERT_TRUE(v);
  ASSERT_TRUE(cppcommon::is_container_v<std::set<int>>);
  v = cppcommon::is_container_v<std::map<int, int>>;
  ASSERT_TRUE(v);
  v = cppcommon::is_container_v<const std::map<int, int>>;
  ASSERT_TRUE(v);
  ASSERT_TRUE(cppcommon::is_container_v<std::list<int>>);
  ASSERT_TRUE(cppcommon::is_container_v<std::deque<int>>);
  ASSERT_TRUE(cppcommon::is_container_v<const std::deque<int>>);
  ASSERT_TRUE(cppcommon::is_container_v<const std::deque<int> &>);

  // bad case
  v = cppcommon::is_vector_v<std::array<int, 1>>;
  ASSERT_TRUE(!v);
  v = cppcommon::is_map_v<std::set<int>>;
  ASSERT_TRUE(!v);
  v = cppcommon::is_map_v<std::vector<int>>;
  ASSERT_TRUE(!v);

  ASSERT_TRUE(!cppcommon::is_container_v<std::string>);
  ASSERT_TRUE(!cppcommon::is_container_v<int>);
  ASSERT_TRUE(!cppcommon::is_container_v<char *>);
  // queue is container adapter
  ASSERT_TRUE(!cppcommon::is_container_v<std::queue<int>>);

  // pointer type
  v = cppcommon::is_container_v<const std::map<int, int> *>;
  ASSERT_TRUE(!v);
}
