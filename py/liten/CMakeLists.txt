# Top level CMake file
cmake_minimum_required(VERSION 3.22)
project(liten VERSION "${LITEN_BASE_VERSION}")

# Running from a Python sdist tarball
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$ENV{LITEN_ROOT_DIR}/cpp/cmake_modules")

#include($ENV{LITEN_ROOT_DIR}/cpp/cmake_modules/BuildUtils.cmake)
include(BuildUtils)

#include(CMakeParseArguments)

#include(BuildUtils)
find_package(Python3Alt REQUIRED)
include(UseCython)
# pick BuildUtils from cpp
#
include(BuildUtils)
message(STATUS "python include: ${PYTHON_INCLUDE_DIRS} ${NUMPY_INCLUDE_DIRS}")

set(module lib)
set(module_name ${module})
# Change these to pick TBD Use $CMAKE_BUILD_TYPE here
#
link_directories(${LITEN_ROOT_DIR}/cpp/build/${CMAKE_BUILD_TYPE}/bin)
#
include_directories(${LITEN_ROOT_DIR}/cpp/cache_test ${LITEN_ROOT_DIR}/cpp/cache ${LITEN_ROOT_DIR}/cpp/common)
include_directories(${LITEN_ROOT_DIR}/cpp/external_libs/arrow/arrow/cpp/src)
include_directories(${LITEN_ROOT_DIR}/py/liten/liten)
include_directories(${NUMPY_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
set(LINK_LIBS liten)

set(module_SRC liten/${module}.pyx)
set_source_files_properties(${module_SRC} PROPERTIES CYTHON_IS_CXX 1)
cython_add_module(${module_name} ${module_name}_pyx ${module_name}_output ${module_SRC})
target_link_libraries(${module_name} PRIVATE ${LINK_LIBS})

# Generated files will be moved to the right directory by setup.py.

