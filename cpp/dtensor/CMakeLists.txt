# Top level CMake file
cmake_minimum_required(VERSION 3.17)
message(STATUS "Building using CMake version: ${CMAKE_VERSION}")
#
# Create all object files
#
add_library(dtensor_objects OBJECT TCatalog.cpp TCache.cpp TColumnMap.cpp TTable.cpp)
target_include_directories(dtensor_objects PUBLIC ${COMMON_SRC}  ${DTENSOR_SRC})
#
# dtensor static library for executables
#
add_library(dtensor STATIC $<TARGET_OBJECTS:dtensor_objects>)
target_link_libraries(dtensor general common arrow)
target_link_libraries(dtensor debug glogd)
target_link_libraries(dtensor optimized glog)
