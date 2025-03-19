cmake_minimum_required(VERSION 3.20)
project(cppcommon)
set(CMAKE_CXX_STANDARD 17)
add_definitions(-Wall)

include_directories(include)

aux_source_directory(src SRCS)
aux_source_directory(src/io/file SRCS)

add_library(${PROJECT_NAME} ${SRCS})
target_include_directories(
  ${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
                         "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
set_target_properties(${PROJECT_NAME} PROPERTIES)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# install files
include(GNUInstallDirs) # CMAKE_INSTALL_INCLUDEDIR / CMAKE_INSTALL_LIBDIR / ...
install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY include/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(
  EXPORT ${PROJECT_NAME}
  DESTINATION share/${PROJECT_NAME}
  FILE ${PROJECT_NAME}Config.cmake
  NAMESPACE ${PROJECT_NAME}::)
