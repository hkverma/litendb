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
from .includes.ctcache cimport *

from graphviz import Digraph
from graphviz import Source

import sys
import codecs

from .utils import to_bytes

cdef class TSchema:
    """
    Liten Schema Class
    """

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
        return to_bytes(name)

    def get_info(self):
        """
        Returns
          unique name of the table
        """
        schema_str = self.p_tschema.ToString()
        return to_bytes(schema_str)

    def get_type(self):
        """
        Returns
          Dimension or Fact Table
        """
        ttype = self.p_tschema.GetType()
        if (ttype == DimensionTable):
            return DimensionTable
        else:
            return FactTable

    def get_field_type(self, field_name):
        """
        Get field type for field_name
        Parameters
          field_name name of field
        Returns
          Dimension or Metric or Feature or Embedding Field types. exception if failed to get it.
        """
        cdef:
           CTResultFieldType ftype_result
           FieldType ftype
        ftype_result = self.p_tschema.GetFieldType(to_bytes(field_name))
        if (not ftype_result.ok()):
            raise ValueError("Failed to get field by msg={ftype_result.status.message()}")

        ftype = ftype_result.ValueOrDie()
        if (ftype == DimensionField):
            return DimensionField
        elif (FeatureField == ftype):
            return FeatureField
        elif (EmbeddingField == ftype):
            return EmbeddingField
        else:
            return MetricField

    def set_field_type(self, field_name, field_type):
        """
        Get field type for field_name
        Parameters
          field_name name of field
          field_type Dimension or Metric or Feature or Embedding Field types
        Returns
          True if set else False
        """
        cdef:
           FieldType ftype
           CTStatus status
        ftype = MetricField
        if (field_type == DimensionField):
            ftype = DimensionField
        elif (field_type == FeatureField):
            ftype = FeatureField
        if (field_type == EmbeddingField):
            ftype = EmbeddingField
        status = self.p_tschema.SetFieldType(to_bytes(field_name), ftype)
        if ( not status.ok()):
            print(f"Failed to set field with msg={status.message()}")
            return False
        return True

    def join(self, field_name, parent_schema, parent_field_name):
        """
        joints child field with parent field which creates data tensor dimensionality
        Parameters
           field_name name of child field
           parent_schema TSchema of parent
           parent_field_name name of parent field
        Returns
           True if success else False
        """
        if (not type(parent_schema) is TSchema):
            print(f"parent_schema {type(parent_schema)} must be TSchema")
            return False
        p_parent_schema = <TSchema>parent_schema
        status = self.p_tschema.Join(to_bytes(field_name), p_parent_schema.sp_tschema, to_bytes(parent_field_name))
        if (status.ok()):
            return True
        else:
            print(status.message())
            return False
