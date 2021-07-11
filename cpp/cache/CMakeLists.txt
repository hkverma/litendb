# Top level CMake file
cmake_minimum_required(VERSION 3.17)
message(STATUS "Building using CMake version: ${CMAKE_VERSION}")
#
# Create all object files
#
add_library(cache_objects OBJECT TCatalog.cpp TCache.cpp TBlock.cpp TColumn.cpp TRowBlock.cpp TTable.cpp TColumnMap.cpp )
target_include_directories(cache_objects PUBLIC ${COMMON_SRC}  ${CACHE_SRC})
#
# cache static library for executables
#
add_library(cache STATIC $<TARGET_OBJECTS:cache_objects>)
target_link_libraries(cache general common arrow)
target_link_libraries(cache debug glogd)
target_link_libraries(cache optimized glog)
