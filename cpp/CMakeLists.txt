# Top level CMake file
cmake_minimum_required(VERSION 3.23)
#
# Running from a Python sdist tarball

include($ENV{LITEN_ROOT_DIR}/cpp/cmake_modules/BuildUtils.cmake)

project(liten VERSION "${LITEN_BASE_VERSION}")
#
# Subdirectories
#
add_subdirectory(common)
add_subdirectory(cache)
add_subdirectory(cache_test)

# 
# liten dynamic library - .so
#
add_library(liten SHARED $<TARGET_OBJECTS:common_objects> $<TARGET_OBJECTS:cache_objects> $<TARGET_OBJECTS:cache_test_objects>)
target_include_directories(liten PUBLIC cache cache_test)
target_link_libraries(liten general common arrow)
target_link_libraries(liten debug glogd debug tbb_debug)
target_link_libraries(liten optimized glog optimized tbb)
