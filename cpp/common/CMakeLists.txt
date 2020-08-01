# Top level CMake file
cmake_minimum_required(VERSION 3.17)
message(STATUS "Building using CMake version: ${CMAKE_VERSION}")
#
# Subdirectories
#
add_library(common TimeUtils.cpp Status.cpp)
target_include_directories(common PUBLIC ${COMMON_SRC})
