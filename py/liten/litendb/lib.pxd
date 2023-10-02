# disutils: language = c++
# cython: language_level = 3
"""
Python C-wrapper CLiten for C++ Liten Cache
"""
from cpython cimport PyObject
from cpython cimport function

from libc.stdint cimport *
from libcpp cimport bool as c_bool, nullptr
from libcpp.functional cimport function
from libcpp.memory cimport shared_ptr, unique_ptr, make_shared
from libcpp.string cimport string as c_string
from libcpp.utility cimport pair
from libcpp.vector cimport vector
from libcpp.unordered_map cimport unordered_map
from libcpp.unordered_set cimport unordered_set

from pyarrow.includes.common cimport *
from pyarrow.includes.libarrow cimport *
from .includes.ctcache cimport *

cdef class TSchema:
     """
     Liten Schema Class wrapping cppclass CTSchema
     """
     cdef:
         shared_ptr[CSchema] sp_pa_schema
         shared_ptr[CTSchema] sp_tschema
         CTSchema* p_tschema
         object tcache

cdef class TTable:
     """
     Liten Table Class wrapping cppclass CTTable
     """
     cdef:
        shared_ptr[CTable] sp_pa_table
        shared_ptr[CTTable] sp_ttable
        CTTable* p_ttable
        object tcache

cdef class TCache:
   """
   Liten TCache wrapping cppclass CTCache with user methods
   """
   cdef:
       shared_ptr[CTCache] sp_tcache
       CTCache* tcache

cdef class TService:
   """
   Liten TService wrapping cppclass CTService with user methods
   """
   cdef:
       shared_ptr[CTService] sp_tservice
       CTService* tservice
