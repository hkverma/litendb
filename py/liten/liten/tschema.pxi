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

cdef class TSchema:
    """
    Liten Schema Class
    """
    
    def __init__(self):
        raise RuntimeError("Error: Always create liten ttable using TCache::add_schema")

    def __init__(self, tc):
        self.tcache = tc
    
    def get_pyarrow_schema(self):
        """
        Get pyarrow schema from Liten schema
        """
        pa_schema = pyarrow_wrap_schema(self.sp_pa_schema)
        return pa_schema
    
    def get_name(self):
        """
        Returns
          unique name of the table
        """
        name = self.sp_tschema.get().GetName()
        return cliten.litenutils.to_bytes(name)

    def get_info(self):
        """
        Returns
          unique name of the table
        """
        schema_str = self.p_tschema.ToString()
        return cliten.litenutils.to_bytes(schema_str)
    
    def get_type(self):
        """
        Returns
          Dimension or Fact Table
        """
        return self.ttype

    def get_type(self):
        """
        Returns
          Dimension or Fact Table
        """
        ttype = self.p_tschema.GetType()
        if (ttype == DimensionTable):
            return self.tcache.DimensionTable
        else:
            return self.tcache.FactTable
        
    def join(self, field_name, parent_schema, parent_field_name):
        if (not type(parent_schema) is TSchema):
            print(f"parent_schema {type(parent_schema)} must be TSchema")
            return False
        p_parent_schema = <TSchema>parent_schema
        status = self.p_tschema.Join(field_name, p_parent_schema.sp_tschema, parent_field_name)
        if (status.ok()):
            return True
        else:
            print(status.message())
            return False
