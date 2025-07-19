find_package(GTest REQUIRED)
enable_testing()

file(GLOB_RECURSE TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/gtest/*.cc")
add_executable(gtest_main ${TEST_SOURCES})
target_link_libraries(gtest_main ${PROJECT_NAME} ${THIRD_LIBRARIES}
                      GTest::GTest GTest::Main)
