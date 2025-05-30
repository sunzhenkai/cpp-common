find_package(GTest REQUIRED)
enable_testing()

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/gtest OS_TEST_SOURCES)
add_executable(os_gtest_main ${OS_TEST_SOURCES})
target_link_libraries(os_gtest_main ${OBJECT_STORAGE_LIB} ${OS_THIRD_LIBRARIES}
                      GTest::GTest GTest::Main)
