# cython: profile=False
# distutils: language = c++
# cython: embedsignature = True
# cython: language_level = 3

from cython.operator cimport dereference as deref
from pyarrow.includes.libarrow cimport *
from tendb.includes.dtensor cimport *

cdef class CTenDB:
    def __cinit__(self):
        self.tcache = NULL

    def __init__(self):
        raise TypeError("Do not call CTenDB's constructor directly, use one of "
                        "the `CTenDB.from_*` functions instead.")   
        
    cdef add_table(self, c_string name, shared_ptr[CTable] table):
        self.sp_tcache = CTCache.GetInstance()
        self.tcache = self.sp_tcache.get()
        self.tcache.AddTable(name, table)
