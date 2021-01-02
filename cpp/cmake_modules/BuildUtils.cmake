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
#
# Set global compile options
#
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
#
# Liten RootDir
#
set (LITEN_ROOT_DIR $ENV{LITEN_ROOT_DIR})

# Bin directories
#
set(LITEN_BIN ${LITEN_ROOT_DIR}/build/bin)
#
# TBB include directory
#
set(TBB_INC_DIR $ENV{LITEN_ROOT_DIR}/cpp/external_libs/tbb/tbb/include)
set(TBB_LIB_DIR $ENV{LITEN_ROOT_DIR}/cpp/external_libs/tbb/bin)
#
# Source Code directories
#
set(COMMON_SRC  $ENV{LITEN_ROOT_DIR}/cpp/common)
set(DTENSOR_SRC $ENV{LITEN_ROOT_DIR}/cpp/dtensor)
set(DTENSOR_TEST_SRC $ENV{LITEN_ROOT_DIR}/cpp/dtensor_test)
#
# Global include and link directories
# TODO expand include directories
#
include_directories(/usr/include/python3.6)
link_directories(${TBB_LIB_DIR})
