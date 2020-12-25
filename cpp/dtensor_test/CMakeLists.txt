# Top level CMake file
cmake_minimum_required(VERSION 3.17)
message(STATUS "Building using CMake version: ${CMAKE_VERSION}")
#
# Create object files
#
add_library(dtensor_test_objects OBJECT TpchQueries.cpp TpchDemo.cpp TestUtils.cpp)
target_include_directories(dtensor_test_objects PUBLIC ${TBB_INC_DIR} ${COMMON_SRC} ${DTENSOR_SRC} ${DTENSOR_TEST_SRC})
#
# Create Test executable
#
add_executable(dtensor_test $<TARGET_OBJECTS:dtensor_test_objects> TestMain.cpp)
target_include_directories(dtensor_test PUBLIC ${TBB_INC_DIR} ${COMMON_SRC} ${DTENSOR_SRC} ${DTENSOR_TEST_SRC})
target_link_libraries(dtensor_test general dtensor common arrow)
target_link_libraries(dtensor_test optimized glog optimized tbb)
target_link_libraries(dtensor_test debug glogd debug tbb_debug)
