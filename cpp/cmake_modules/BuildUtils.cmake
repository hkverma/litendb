# TODO create debug/release directory
#
# Set Versions
#
message(STATUS "Building using CMake version: ${CMAKE_VERSION}")
#
# Set Liten Version
#
set(LITEN_VERSION "0.0.1")
string(REGEX MATCH "^[0-9]+\\.[0-9]+\\.[0-9]+" LITEN_BASE_VERSION "${LITEN_VERSION}")
#
# Select C++17
#
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#
# if no build build type is specified, default to debug builds
#
if(NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build.")
endif()
message( "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}" )
message( "LITEN_BIN_DIR = ${LITEN_BIN_DIR}" )
if(NOT DEFINED LITEN_BIN_DIR)
  set(LITEN_BIN_DIR $ENV{LITEN_ROOT_DIR}/cpp/build/bin CACHE STRING "Specify the build dir.")
endif()

#
# Set global compile options
#
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
#
# Liten RootDir
#
set (LITEN_ROOT_DIR $ENV{LITEN_ROOT_DIR})

# Copy all libraries and executables in bin dir
#
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LITEN_BIN_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${LITEN_BIN_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LITEN_BIN_DIR})
#
# TBB include directory
#
set(TBB_INC_DIR $ENV{LITEN_ROOT_DIR}/cpp/external_libs/tbb/tbb/include)
set(TBB_LIB_DIR $ENV{LITEN_ROOT_DIR}/cpp/external_libs/tbb/bin)
#
# Source Code directories
#
set(COMMON_SRC  $ENV{LITEN_ROOT_DIR}/cpp/common)
set(CACHE_SRC $ENV{LITEN_ROOT_DIR}/cpp/cache)
set(CACHE_TEST_SRC $ENV{LITEN_ROOT_DIR}/cpp/cache_test)
#
# Global include and link directories
# TODO expand include directories
#
include_directories(/usr/include/python3.6)
link_directories(${TBB_LIB_DIR})

