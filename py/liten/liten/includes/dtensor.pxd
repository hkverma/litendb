# disutils: language = c++
# cython: language_level = 3
"""
Cython interface class to C++ Liten Cache and tables
"""
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

cdef extern from "dtensor.h" namespace "liten" nogil:

# CTTable is liten::TTable in Cython. CTable is arrow::Table cython from pyarrow.
   cdef cppclass CTTable" liten::TTable":
      ctypedef enum TType: Dim, Fact
      c_string GetName()
      shared_ptr[CTable] GetTable()
      shared_ptr[CTable] Slice(int64_t offset, int64_t length)
      
   cdef cppclass CTCache" liten::TCache":
      shared_ptr[CTTable] AddTable(c_string name, shared_ptr[CTable] table, CTTable.TType)
      shared_ptr[CTTable] GetTable(c_string name)
      @staticmethod
      shared_ptr[CTCache] GetInstance()
      c_string GetInfo()
      int MakeMaps(c_string name)
      int MakeMaps()
      shared_ptr[CTable] Slice(c_string tableName, int64_t offset, int64_t length)

cdef extern from "TpchDemo.h" namespace "liten" nogil:
   cdef cppclass CTpchDemo" liten::TpchDemo":
       @staticmethod
       shared_ptr[CTpchDemo] GetInstance(shared_ptr[CTCache] tCache)
       double Query6()
       shared_ptr[unordered_map[c_string, double]] Query5()
