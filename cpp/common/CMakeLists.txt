# Top level CMake file
cmake_minimum_required(VERSION 3.17)
message(STATUS "Building using CMake version: ${CMAKE_VERSION}")
#
# Subdirectories
#
add_library(common TTimeUtils.cpp TStatus.cpp TLog.cpp TService.cpp TGuid.cpp TConfigs.cpp)
target_include_directories(common PUBLIC ${COMMON_SRC})
