# Top level CMake file
cmake_minimum_required(VERSION 3.17)
message(STATUS "Building using CMake version: ${CMAKE_VERSION}")
set(NVECTOR_SRC ${TENDB_ROOT_DIR}/cpp/nvec/src)
set(NVECTOR_INC ${TENDB_ROOT_DIR}/cpp/nvec/inc)
#
# Subdirectories
#
add_executable(nvec src/TestMain.cpp)
target_include_directories(nvec PUBLIC ${NVECTOR_INC})
target_link_libraries(nvec arrow)
