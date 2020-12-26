# disutils: language = c++
# cython: language_level = 3

from libc.stdint cimport *
from libcpp cimport bool as c_bool, nullptr
from libcpp.functional cimport function
from libcpp.memory cimport shared_ptr, unique_ptr, make_shared
from libcpp.string cimport string as c_string
from libcpp.utility cimport pair
from libcpp.vector cimport vector
from libcpp.unordered_map cimport unordered_map
from libcpp.unordered_set cimport unordered_set

from pyarrow.includes.libarrow cimport *

cdef extern from "dtensor.h" namespace "tendb" nogil:

   cdef cppclass CTTable" tendb::TTable":
      c_string GetName()
      
   cdef cppclass CTCache" tendb::TCache":
       shared_ptr[CTTable] AddTable(c_string name, shared_ptr[CTable] table)
       @staticmethod
       shared_ptr[CTCache] GetInstance()
       c_string GetInfo()

cdef extern from "TpchDemo.h" namespace "tendb" nogil:
   cdef cppclass CTpchDemo" tendb::TpchDemo":
       @staticmethod
       shared_ptr[CTpchDemo] GetInstance(shared_ptr[CTCache] tCache)
       double Query6()
       shared_ptr[unordered_map[c_string, double]] Query5()
