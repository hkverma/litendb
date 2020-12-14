# Top level CMake file
cmake_minimum_required(VERSION 3.17)
project(tendb VERSION "${TENDB_BASE_VERSION}")

# Running from a Python sdist tarball
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$ENV{TENDB_ROOT_DIR}/cpp/cmake_modules")

#include($ENV{TENDB_ROOT_DIR}/cpp/cmake_modules/BuildUtils.cmake)
include(BuildUtils)

#include(CMakeParseArguments)

#include(BuildUtils)
find_package(Python3Alt)
include(UseCython)

set(CYTHON_EXTENSIONS,
  'lib',
  '_tendb')

set(LINK_LIBS arrow_shared tendb_shared)

foreach(module ${CYTHON_EXTENSIONS})
  set(module_SRC tendb/${module}.pyx)
  set_source_files_properties(${module_SRC} PROPERTIES CYTHON_IS_CXX 1)

  cython_add_module(${module_name} ${module_name}_pyx ${module_name}_output ${module_SRC})

  target_link_libraries(${module_name} PRIVATE ${LINK_LIBS})

  # Generated files will be moved to the right directory by setup.py.
endforeach(module)
