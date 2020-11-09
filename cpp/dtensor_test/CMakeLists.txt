# Top level CMake file
cmake_minimum_required(VERSION 3.17)
message(STATUS "Building using CMake version: ${CMAKE_VERSION}")
#
# Subdirectories
#
add_executable(dtensor_test TpchQueries.cpp TestUtils.cpp TestMain.cpp)
target_include_directories(dtensor_test PUBLIC ${TBB_INC_DIR} ${COMMON_SRC} ${DTENSOR_SRC} ${DTENSOR_TEST_SRC})
target_link_directories(dtensor_test PUBLIC ${TBB_LIB_DIR})
target_link_libraries(dtensor_test general dtensor common arrow)
target_link_libraries(dtensor_test optimized glog tbb)
target_link_libraries(dtensor_test debug glogd tbb_debug)
