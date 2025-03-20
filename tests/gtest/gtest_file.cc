#include <string_view>

#include "cppcommon/io/file/rw.h"
#include "gtest/gtest.h"

TEST(File, ReadByLine) {
  std::string example_file = "a.txt";
  cppcommon::WriteFile(example_file.c_str(), "a\nb");
  cppcommon::ReadFileByLine(example_file.c_str(), [](size_t line_no, std::string_view sv) {
    std::cout << "line_no: " << line_no << ", line_content: " << sv << std::endl;
  });
}
