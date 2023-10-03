"""
Liten Schema
"""
import litendb.lib as cliten

import sys
import codecs

class Schema:
    """
    Liten Schema Class
    """
    def __init__(self, tschema):
        """
        Create and initialize Liten Cache
        """
        if (None == tschema):
            raise ValueError("Invalid value for Liten schema")
        self.tschema = tschema
    
    def get_pyarrow_schema(self):
        """
        Get pyarrow schema from Liten schema
        """
        return self.tschema.get_pyarrow_schema()
    
    def get_name(self):
        """
        Returns
          unique name of the table
        """
        return self.tschema.get_name()

    def get_info(self):
        """
        Returns
          unique name of the table
        """
        return self.tschema.get_info()
    
    def get_type(self):
        """
        Returns
          Dimension or Fact Table
        """
        return self.tschema.get_type()

    def get_field_type(self, field_name):
        """
        Get field type for field_name
        Parameters
          field_name name of field
        Returns
          Dimension or Metric or Feature or DerivedFeature Field types. None if failed to get it.
        """
        return self.tschema.get_field_type(field_name)

    def set_field_type(self, field_name, field_type):
        """
        Get field type for field_name
        Parameters
          field_name name of field
          field_type Dimension or Metric or Feature or DerivedFeature Field types
        Returns
          True if set else False
        """
        return self.tschema.set_field_type(field_name, field_type)
            
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
        return self.tschema.join(field_name, parent_schema, parent_field_name)
