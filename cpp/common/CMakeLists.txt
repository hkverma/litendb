# Top level CMake file
cmake_minimum_required(VERSION 3.17)
message(STATUS "Building using CMake version: ${CMAKE_VERSION}")
#
# Subdirectories
#
add_library(common_objects OBJECT TTimeUtils.cpp TStatus.cpp TLog.cpp TService.cpp TGuid.cpp TTaskScheduler.cpp TConfigs.cpp TResult.cpp)
target_include_directories(common_objects PUBLIC ${TBB_INC_DIR} ${COMMON_SRC})
#
# common static library for executables
#
add_library(common STATIC $<TARGET_OBJECTS:common_objects>)
