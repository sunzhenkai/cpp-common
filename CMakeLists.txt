PROJECT(cpp-common)
CMAKE_MINIMUM_REQUIRED(VERSION 3.20)
SET(CMAKE_CXX_STANDARD 20)
ADD_DEFINITIONS(-Wall -g)
SET(CPP_COMMON_VERSION 0.0.7)
INCLUDE(GNUInstallDirs) # CMAKE_INSTALL_INCLUDEDIR / CMAKE_INSTALL_LIBDIR / ...

INCLUDE_DIRECTORIES(include)

SET(SRCS)
AUX_SOURCE_DIRECTORY(src SRCS)
AUX_SOURCE_DIRECTORY(src/file SRCS)

ADD_LIBRARY(${PROJECT_NAME} ${SRCS})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES VERSION ${CPP_COMMON_VERSION})
ADD_LIBRARY(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# install files
INSTALL(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
INSTALL(DIRECTORY include/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

# install pkg-config file
SET(PKG_CONFIG ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc)
SET(PKG_CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/pkgconfig")

IF (IS_ABSOLUTE "${CMAKE_INSTALL_INCLUDEDIR}")
    SET(PKG_CONFIG_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}")
ELSE ()
    SET(PKG_CONFIG_INCLUDEDIR "\${prefix}/${CMAKE_INSTALL_INCLUDEDIR}")
ENDIF ()
IF (IS_ABSOLUTE "${CMAKE_INSTALL_LIBDIR}")
    SET(PKG_CONFIG_LIBDIR "${CMAKE_INSTALL_LIBDIR}")
ELSE ()
    SET(PKG_CONFIG_LIBDIR "\${exec_prefix}/${CMAKE_INSTALL_LIBDIR}")
ENDIF ()
CONFIGURE_FILE("cmake/${PROJECT_NAME}.pc.in" "${PKG_CONFIG}" @ONLY)
INSTALL(FILES "${PKG_CONFIG}" DESTINATION "${PKG_CONFIG_INSTALL_DIR}")

# install CMake config files
INCLUDE(CMakePackageConfigHelpers)
MESSAGE(STATUS "CMAKE_INSTALL_DATAROOTDIR=${CMAKE_INSTALL_DATAROOTDIR} CMAKE_CURRENT_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}")
SET(EXPORT_DEST_DIR "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}")
SET(VERSION_CONFIG ${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake)
SET(PROJECT_CONFIG_TARGETS "${PROJECT_NAME}ConfigTargets.cmake")
SET(PROJECT_CONFIG_IN "${CMAKE_CURRENT_LIST_DIR}/cmake/${PROJECT_NAME}Config.cmake.in")
SET(PROJECT_CONFIG_OUT "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake")

EXPORT(TARGETS ${PROJECT_NAME} NAMESPACE ${PROJECT_NAME}:: FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_CONFIG_TARGETS}")
INSTALL(EXPORT ${PROJECT_NAME} DESTINATION ${EXPORT_DEST_DIR} NAMESPACE ${PROJECT_NAME}:: FILE ${PROJECT_CONFIG_TARGETS})
CONFIGURE_PACKAGE_CONFIG_FILE(
        "${PROJECT_CONFIG_IN}"
        "${PROJECT_CONFIG_OUT}"
        INSTALL_DESTINATION ${EXPORT_DEST_DIR})
WRITE_BASIC_PACKAGE_VERSION_FILE("${VERSION_CONFIG}" VERSION ${CPP_COMMON_VERSION} COMPATIBILITY AnyNewerVersion)
INSTALL(FILES "${PROJECT_CONFIG_OUT}" "${VERSION_CONFIG}" DESTINATION "${EXPORT_DEST_DIR}")
