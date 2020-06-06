# Top level CMake file
cmake_minimum_required(VERSION 3.17)
message(STATUS "Building using CMake version: ${CMAKE_VERSION}")
#
# Subdirectories
#
add_executable(dtensor TestMain.cpp)
target_include_directories(dtensor PUBLIC ${DTENSOR_SRC})
target_link_libraries(dtensor arrow)
