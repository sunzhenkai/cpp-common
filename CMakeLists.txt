cmake_minimum_required(VERSION 3.20)
project(cppcommon)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_definitions(-Wall)
include(GNUInstallDirs) # CMAKE_INSTALL_INCLUDEDIR / CMAKE_INSTALL_LIBDIR / ...

if(BUILD_TESTING)
  # set testing install prefix
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/build/install/)
endif()

include_directories(.)
file(GLOB_RECURSE LIB_HREADERS cppcommon/*.h)
file(GLOB_RECURSE LIB_SRCS cppcommon/*.cc)

add_library(${PROJECT_NAME} ${LIB_HREADERS} ${LIB_SRCS})
target_include_directories(
  ${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
                         "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

# install files
install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(
  DIRECTORY cppcommon
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
  FILES_MATCHING
  PATTERN "*.h")
install(
  EXPORT ${PROJECT_NAME}
  DESTINATION share/${PROJECT_NAME}
  FILE ${PROJECT_NAME}Config.cmake
  NAMESPACE ${PROJECT_NAME}::)
