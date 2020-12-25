# cython: profile=False
# distutils: language = c++
# cython: embedsignature = True
# cython: language_level = 3

from cython.operator cimport dereference as deref
from pyarrow.includes.libarrow cimport *
from pyarrow.lib cimport *
from tendb.includes.dtensor cimport *

cdef class CTenDB:
    def __cinit__(self):
        self.tcache = NULL

    def __init__(self):
        self.tcache = NULL
        self.sp_tcache = CTCache.GetInstance()
        self.tcache = self.sp_tcache.get()
        print("Added a new cache")

    def show_versions(self):
        return "0.0.2"

    def add_table(self, name, table):
        cdef:
            shared_ptr[CTable] sp_table
            shared_ptr[CTTable] sp_ttable
            CTTable* p_ttable
        sp_table = pyarrow_unwrap_table(table)
        sp_ttable = self.tcache.AddTable(name, sp_table)
        p_ttable = sp_ttable.get()
        return p_ttable.GetName()

    @property
    def version(self):
        return "0.0.2"
