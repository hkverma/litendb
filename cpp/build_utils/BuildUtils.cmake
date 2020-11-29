#
# Set global compile options
#
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
#
# Bin directories
#
set(TENDB_BIN $ENV{TENDB_ROOT_DIR}/build/bin)
#
# TBB include directory
#
set(TBB_INC_DIR $ENV{TENDB_ROOT_DIR}/cpp/external_libs/tbb/tbb/include)
set(TBB_LIB_DIR $ENV{TENDB_ROOT_DIR}/cpp/external_libs/tbb/bin)
#
# Source Code directories
#
set(COMMON_SRC  $ENV{TENDB_ROOT_DIR}/cpp/common)
set(DTENSOR_SRC $ENV{TENDB_ROOT_DIR}/cpp/dtensor)
set(DTENSOR_TEST_SRC $ENV{TENDB_ROOT_DIR}/cpp/dtensor_test)
#
# Global include and link directories
# TODO expand include directories
#
include_directories(/usr/include/python3.6)
link_directories(${TBB_LIB_DIR})
