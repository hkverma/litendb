# Top level CMake file
cmake_minimum_required(VERSION 3.17)
message(STATUS "Building using CMake version: ${CMAKE_VERSION}")

set(TENDB_VERSION "0.0.1-SNAPSHOT")
string(REGEX MATCH "^[0-9]+\\.[0-9]+\\.[0-9]+" TENDB_BASE_VERSION "${TENDB_VERSION}")

# if no build build type is specified, default to debug builds
if(NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build.")
endif()

project(tendb VERSION "${ARROW_BASE_VERSION}")
include(BuildUtils/BuildUtils.cmake)
#
# Subdirectories
#
add_subdirectory(nvec)
