enable_testing()
find_package(GTest CONFIG REQUIRED)

set(GTESTS_SRCS ${SRCS_CORE})
aux_source_directory(${PROJECT_SOURCE_DIR}/tests/gtests GTESTS_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/tests/gtests/common GTESTS_SRCS)
add_executable(GTestMain ${GTESTS_SRCS})
set(GTEST_LIBS GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
target_link_libraries(GTestMain PUBLIC
        ${COMMON_LIBS}
        ${GTEST_LIBS}
)

add_test(AllTestsInMain GTestMain)
