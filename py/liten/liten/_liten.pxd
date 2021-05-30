# disutils: language = c++
# cython: language_level = 3
"""
Python C-wrapper CLiten for C++ Liten Cache
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
from liten.includes.dtensor cimport *

cdef class CLiten:
   """
   Python C-wrapper CLiten for C++ Liten Cache
   """
   cdef:
       shared_ptr[CTCache] sp_tcache
       CTCache* tcache
