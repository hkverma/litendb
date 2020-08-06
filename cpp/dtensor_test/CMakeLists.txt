# Top level CMake file
cmake_minimum_required(VERSION 3.17)
message(STATUS "Building using CMake version: ${CMAKE_VERSION}")
#
# Subdirectories
#
add_executable(dtensor_test TpchQueries.cpp TestUtils.cpp TestMain.cpp)
target_include_directories(dtensor PUBLIC ${COMMON_SRC} ${DTENSOR_SRC} ${DTENSOR_TEST_SRC})
target_link_libraries(dtensor_test general dtensor common arrow)
target_link_libraries(dtensor debug glogd)
target_link_libraries(dtensor optimized glog)
