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
from libcpp cimport bool

from pyarrow.includes.libarrow cimport *

cdef extern from "common.h" namespace "liten" nogil:
# CTStatus is liten::TStatus
   cdef cppclass CTStatus" liten::TStatus":
      bool ok() const
      c_string message() const
      
# CTResult is liten::CTResult
   cdef cppclass CTResultCTTable" liten::TResult<std::shared_ptr<liten::TTable>>":
      bool ok() const
      const shared_ptr[CTTable]& ValueOrDie() const

cdef extern from "cache.h" namespace "liten" nogil:

   ctypedef enum TableType: DimensionTable, FactTable
   
# CTTable is liten::TTable in Cython. CTable is arrow::Table cython from pyarrow.
   cdef cppclass CTTable" liten::TTable":
      c_string GetName()
      shared_ptr[CTable] GetTable()
      shared_ptr[CTable] Slice(int64_t offset, int64_t length)

# CTCache is liten::TCache      
   cdef cppclass CTCache" liten::TCache":
      CTResultCTTable AddTable(c_string tableName, TableType type, shared_ptr[CTable] table)
      shared_ptr[CTTable] GetTable(c_string name) const
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
