cmake_minimum_required(VERSION 3.20)
project(cpp-common)
set(CMAKE_CXX_STANDARD 17)

add_definitions(-Wall -g)
include(3rd/vcpkg-base/cmakes/tools.cmake)
include_directories(include)

option(BUILD_NET "build net library" ON)
option(BUILD_TESTS "build net library" OFF)

find_package(fmt CONFIG REQUIRED)
set(COMMON_LIBS fmt::fmt)

# core
file(GLOB_RECURSE SRCS_CORE src/core/**.cpp)
add_library(core ${SRCS_CORE})
target_link_libraries(core ${COMMON_LIBS})
install(DIRECTORY include/cxxcommon/common/ DESTINATION include/cxxcommon/common/)
AddInstall(PROJECT ${PROJECT_NAME} EXPORT_TARGET core INCLUDE_DIR include/cxxcommon/core/)

# net
if (BUILD_NET)
    find_package(CURL REQUIRED)

    set(NET_LIBS CURL::libcurl)
    file(GLOB_RECURSE SRCS_CORE src/net/**.cpp)
    add_library(net ${SRCS_CORE})
    target_link_libraries(net ${COMMON_LIBS} ${NET_LIBS})
    AddInstall(PROJECT ${PROJECT_NAME} EXPORT_TARGET net INCLUDE_DIR include/cxxcommon/net/)
endif ()

if (BUILD_TESTS)
    include(cmakes/gtest_main.cmake)
endif ()