# Top level CMake file
cmake_minimum_required(VERSION 3.23)
message(STATUS "Building using CMake version: ${CMAKE_VERSION}")
#
# Create object files
#
add_library(cache_test_objects OBJECT TpchDemo.cpp TestUtils.cpp)
target_include_directories(cache_test_objects PUBLIC ${TBB_INC_DIR} ${COMMON_SRC} ${CACHE_SRC} ${CACHE_TEST_SRC})
#
# Create Test executable
#
add_executable(cache_test $<TARGET_OBJECTS:cache_test_objects> TestMain.cpp)
target_include_directories(cache_test PUBLIC ${TBB_INC_DIR} ${COMMON_SRC} ${CACHE_SRC} ${CACHE_TEST_SRC})
target_link_libraries(cache_test general cache common arrow)
target_link_libraries(cache_test optimized glog optimized tbb)
target_link_libraries(cache_test debug glogd debug tbb_debug)
