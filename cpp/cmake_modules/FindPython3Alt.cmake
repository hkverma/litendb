set(Python3_FIND_STRATEGY "LOCATION")

find_package(Python3 COMPONENTS Interpreter Development NumPy REQUIRED)

if(NOT Python3_FOUND)
  return()
endif()

set(PYTHON_EXECUTABLE ${Python3_EXECUTABLE})
set(PYTHON_INCLUDE_DIRS ${Python3_INCLUDE_DIRS})
set(PYTHON_LIBRARIES ${Python3_LIBRARIES})
set(PYTHON_OTHER_LIBS)
