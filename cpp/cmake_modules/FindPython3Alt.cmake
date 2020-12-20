#
# Only with CMake > 3.15
# From arrow/arrow/cpp/cmake_modules/FindPython3Alt.cmake
#
set(Python3_FIND_STRATEGY "LOCATION")

find_package(Python3 COMPONENTS Interpreter Development NumPy REQUIRED)

if(NOT Python3_FOUND)
  return()
endif()

set(PYTHON_EXECUTABLE ${Python3_EXECUTABLE})
set(PYTHON_INCLUDE_DIRS ${Python3_INCLUDE_DIRS})
set(PYTHON_LIBRARIES ${Python3_LIBRARIES})
set(PYTHON_OTHER_LIBS)

get_target_property(NUMPY_INCLUDE_DIRS Python3::NumPy INTERFACE_INCLUDE_DIRECTORIES)

# CMake's python3_add_library() doesn't apply the required extension suffix,
# detect it ourselves.
# (https://gitlab.kitware.com/cmake/cmake/issues/20408)
execute_process(
  COMMAND "${PYTHON_EXECUTABLE}" "-c"
          "from distutils import sysconfig; print(sysconfig.get_config_var('EXT_SUFFIX'))"
  RESULT_VARIABLE _PYTHON_RESULT
  OUTPUT_VARIABLE _PYTHON_STDOUT
  ERROR_VARIABLE _PYTHON_STDERR)

if(NOT _PYTHON_RESULT MATCHES 0)
  if(Python3Alt_FIND_REQUIRED)
    message(FATAL_ERROR "Python 3 config failure:\n${_PYTHON_STDERR}")
  endif()
endif()

string(STRIP ${_PYTHON_STDOUT} _EXT_SUFFIX)

function(PYTHON_ADD_MODULE name)
  python3_add_library(${name} MODULE ${ARGN})
  set_target_properties(${name} PROPERTIES SUFFIX ${_EXT_SUFFIX})
endfunction()

find_package_handle_standard_args(Python3Alt
                                  REQUIRED_VARS
                                  PYTHON_EXECUTABLE
                                  PYTHON_LIBRARIES
                                  PYTHON_INCLUDE_DIRS
                                  NUMPY_INCLUDE_DIRS)
