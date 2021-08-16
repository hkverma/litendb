# cython: profile=False
# distutils: language = c++
# cython: embedsignature = True
# cython: language_level = 3
"""
CLiten Cache System
"""
from cython.operator cimport dereference as deref, postincrement
from pyarrow.includes.libarrow cimport *
from pyarrow.lib cimport *
from liten.includes.dtensor cimport *

from graphviz import Digraph
from graphviz import Source

import sys
import codecs

import liten as ten
from liten import litenutils

cdef class TTable:
    """
    Liten Table Class
    """
    
    Dimension = 0
    Fact = 1
    ttype = Fact
    
    def __cinit__(self):
        TTable.Dimension = 0
        TTable.Fact = 1
        
    def get_pyarrow_table(self):
        """
        Returns
          Arrow table
        """
        pa_table = pyarrow_wrap_table(self.sp_pa_table)
        return pa_table

    def get_name(self):
        """
        Returns
          unique name of the table
        """
        name = self.p_ttable.GetName()
        return ten.litenutils.to_bytes(name)
    
    def get_type(self):
        """
        Returns
          Dimension or Fact Table
        """
        return self.ttype
