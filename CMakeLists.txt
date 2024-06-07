cmake_minimum_required(VERSION 3.20)
project(cpp-common)
set(CMAKE_CXX_STANDARD 17)

add_definitions(-Wall -g)
include(3rd/vcpkg-base/cmakes/tools.cmake)

include_directories(include)

# core
file(GLOB_RECURSE SRCS_CORE src/**.cpp)
add_library(${PROJECT_NAME} ${SRCS_CORE})
AddInstall(PROJECT ${PROJECT_NAME} INCLUDE_DIR include/cxxcommon/core/)
