# Top level CMake file
cmake_minimum_required(VERSION 3.17)
#
# Running from a Python sdist tarball

include($ENV{TENDB_ROOT_DIR}/cpp/cmake_modules/BuildUtils.cmake)

project(tendb VERSION "${TENDB_BASE_VERSION}")
#
# Subdirectories
#
add_subdirectory(common)
add_subdirectory(dtensor)
add_subdirectory(dtensor_test)

# 
# tendb dynamic library - .so
#
add_library(tendb SHARED $<TARGET_OBJECTS:dtensor_objects> $<TARGET_OBJECTS:dtensor_test_objects>)
target_include_directories(tendb PUBLIC dtensor dtensor_test)
target_link_libraries(tendb general common arrow)
target_link_libraries(tendb debug glogd debug tbb_debug)
target_link_libraries(tendb optimized glog optimized tbb)
