#include <iostream>
#include <string>

#include "cppcommon/utils/simple_config.h"
#include "gtest/gtest.h"

TEST(SimpleConfig, Base) {
  std::string c = "a:1;b:2;pi:3.14";
  cppcommon::SimpleConfig cfg(c, false);

  std::cout << cfg.ToString() << std::endl;
  ASSERT_EQ(1, cfg.GetInt64("a", -1));
  ASSERT_EQ("1", cfg.Get("a", ""));
  ASSERT_EQ("3.14", cfg.Get("pi", ""));
  ASSERT_EQ(3.14, cfg.GetDouble("pi", 0));
  ASSERT_EQ(0, cfg.GetDouble("nan", 0));
  ASSERT_EQ(-1, cfg.GetInt64("nan", -1));
}

TEST(SimpleConfig, BadCase1) {
  std::string c = "a:1;b:2;pi:3.14;c:;w:false;z::a;ac";
  cppcommon::SimpleConfig cfg(c, false);

  std::cout << cfg.ToString() << std::endl;
  ASSERT_EQ(-1, cfg.GetInt64("c", -1));
  ASSERT_EQ("", cfg.Get("c", "nan"));
  ASSERT_EQ("nan", cfg.Get("nn", "nan"));
  ASSERT_EQ(false, cfg.GetBoolean("w", true));

  ASSERT_EQ(0, cfg.GetDouble("nan", 0));
  ASSERT_EQ(-1, cfg.GetInt64("nan", -1));
  ASSERT_EQ(":a", cfg.Get("z", ""));
}

TEST(SimpleConfig, BadCase2) {
  std::string c = "";
  cppcommon::SimpleConfig cfg(c, false);
  ASSERT_EQ(0, cfg.Size());
  ASSERT_EQ(false, cfg.HasError());

  c = ";;;";
  cfg = cppcommon::SimpleConfig(c, false);
  ASSERT_EQ(0, cfg.Size());
  ASSERT_EQ(false, cfg.HasError());

  c = "a:a:v;b:";
  cfg = cppcommon::SimpleConfig(c, false);
  std::cout << cfg.ToString() << std::endl;
}
