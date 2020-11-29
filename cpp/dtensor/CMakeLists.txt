# Top level CMake file
cmake_minimum_required(VERSION 3.17)
message(STATUS "Building using CMake version: ${CMAKE_VERSION}")
#
# Subdirectories
#
add_library(dtensor TCache.cpp TColumnMap.cpp TTable.cpp dtensor_python.cpp)
target_include_directories(dtensor PUBLIC ${COMMON_SRC}  ${DTENSOR_SRC})
target_link_libraries(dtensor general common arrow)
target_link_libraries(dtensor debug glogd)
target_link_libraries(dtensor optimized glog)
