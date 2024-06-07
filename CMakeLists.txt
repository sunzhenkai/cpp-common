cmake_minimum_required(VERSION 3.20)
project(cpp-common)
set(CMAKE_CXX_STANDARD 17)

add_definitions(-Wall -g)
include(3rd/vcpkg-base/cmakes/tools.cmake)
include_directories(include)

option(BUILD_NET "build net library" ON)

find_package(fmt CONFIG REQUIRED)
set(COMMON_LIBS fmt::fmt)

# core
file(GLOB_RECURSE SRCS_CORE src/core/**.cpp)
add_library(core ${SRCS_CORE})
target_link_libraries(core ${COMMON_LIBS})
AddInstall(PROJECT ${PROJECT_NAME} EXPORT_TARGET core INCLUDE_DIR include/cxxcommon/core/)

# net
if (BUILD_NET)
    find_package(CURL REQUIRED)

    file(GLOB_RECURSE SRCS_CORE src/net/**.cpp)
    add_library(net ${SRCS_CORE})
    target_link_libraries(net ${COMMON_LIBS} CURL::libcurl)
    AddInstall(PROJECT ${PROJECT_NAME} EXPORT_TARGET net INCLUDE_DIR include/cxxcommon/core/)
endif ()