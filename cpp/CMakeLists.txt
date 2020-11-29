# Top level CMake file
# TODO create debug/release directory
cmake_minimum_required(VERSION 3.17)
message(STATUS "Building using CMake version: ${CMAKE_VERSION}")

set(TENDB_VERSION "0.0.1-SNAPSHOT")
string(REGEX MATCH "^[0-9]+\\.[0-9]+\\.[0-9]+" TENDB_BASE_VERSION "${TENDB_VERSION}")

# Select C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# if no build build type is specified, default to debug builds
if(NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build.")
endif()

project(tendb VERSION "${TENDB_BASE_VERSION}")
include(build_utils/BuildUtils.cmake)
#
# Subdirectories
#
add_subdirectory(common)
add_subdirectory(dtensor)
add_subdirectory(dtensor_test)

# 
# tendb dynamic library - .so
#
add_library(tendb SHARED $<TARGET_OBJECTS:dtensor_objects>)
target_include_directories(tendb PUBLIC dtensor)
target_link_libraries(tendb general common arrow)
target_link_libraries(tendb debug glogd)
target_link_libraries(tendb optimized glog)
