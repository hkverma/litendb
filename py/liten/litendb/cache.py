"""
Liten Cache
"""
from .schema import Schema
from .table import Table

import pyarrow as pa
from pyarrow import csv

import sys
import codecs

import litendb.lib as cliten

class Cache:
    """
    Liten Cache Class
    """
    tcache = cliten.TCache()
    
    def __init__(self):
        """
        Create and initialize Liten Cache
        """
        pass

    @property
    def version(self):
        return Cache.tcache.version

    @property
    def FactTable(self):
        return Cache.tcache.FactTable

    @property
    def DimensionTable(self):
        return Cache.tcache.DimensionTable
    
    @property
    def DimensionField(self):
        return Cache.tcache.DimensionField

    @property
    def MetricField(self):
        return Cache.tcache.MetricField

    @property
    def FeatureField(self):
        return Cache.tcache.FeatureField

    @property
    def EmbeddingField(self):
        return Cache.tcache.EmbeddingField

    def info(self):
        """
        return cache information including compute and storage 
        Returns
          string containing cache information
        """
        return Cache.tcache.info()

    def compute_info(self):
        """
        return cache information including compute and storage 
        Returns
          string containing cache information
        """
        return Cache.tcache.compute_info()

    def table_info(self):
        """
        return cache information including compute and storage 
        Returns
          string containing cache information
        """
        return Cache.tcache.table_info()

    def get_table_pyarrow(self, name):
        """
        Returns
          Arrow table
        """
        return Cache.tcache.get_table_pyarrow(name)

    def get_table_type(self, name):
        """
        Returns
          Dimension or Fact Table
        """
        return Cache.tcache.get_table_type(name)

    def schema_info(self):
        """
        return cache information including compute and storage 
        Returns
          string containing cache information
        """
        return Cache.tcache.schema_info()
    
    def get_schema_info(self, name):
        """
        return schema information
        Parameter
          name schema name
        Returns
          string containing schema information
        """
        return Cache.tcache.get_schema_info(name)

    def add_schema(self, name, ttype, pa_schema):
        """
        Add arrow table in cache by name
        Parameters
           name: name of schema
           ttype: type of table must be DimensionTable or FactTable
           schema: arrow schema to be added in liten cache
        Returns
           schema name or exception if failed to add
        """
        return Cache.tcache.add_schema(name, ttype, pa_schema)
    
    def if_valid_schema(self, name):
        """
        Is a valid schema by name
        Parameters
           name: name of schema
        Returns
           True if valid, else false
        """
        return Cache.tcache.if_valid_schema(name)

    def add_schema_from_ttable(self, table):
        """
        Add schema associated with ttable by name. If a schema exists by name, that is returned
        Parameters
           ttable: schema in liten table ttable
        Returns
           schema name
        """
        return Cache.tcache.add_schema_from_ttable(table.ttable)

    def get_schema_pyarrow(self, name):
        """
        Get pyarrow schema from Liten schema
        """
        return self.tcache.get_schema_pyarrow(name)
    
    def get_schema_type(self, name):
        """
        Returns
          Dimension or Fact Table
        """
        return self.tcache.get_schema_type(name)

    def get_schema_field_type(self, name, field_name):
        """
        Get field type for field_name
        Parameters
          field_name name of field
        Returns
          Dimension or Metric or Feature or Embedding Field types. None if failed to get it.
        """
        return self.tcache.get_schema_field_type(name, field_name)

    def set_schema_field_type(self, schema_name, field_name, field_type):
        """
        Get field type for field_name
        Parameters
          schema_name
          field_names name of field can be list or a value
          field_types can be list or value, Dimension or Metric or Feature or Embedding Field types
        Returns
          True if set else False or exceptions
        """
        return Cache.tcache.set_schema_field_type(schema_name, field_name, field_type)
    
    def add_table(self, name, pa_table, ttype, schema_name=""):
        """
        Create arrow table in cache by name
        Parameters
           name: name of table
           table: arrow table to be added in liten cache
           ttype: type of table must be DimensionTable or FactTable
        Returns
           table name or exception if not added
        """
        return Cache.tcache.add_table(name, pa_table, ttype, schema_name)

    def if_valid_table(self, name):
        """
        if table by name exists
        Parameters
           name: name of table
        Returns
           True if exists else False
        """
        return Cache.tcache.if_valid_table(name)
        
    def make_tensor_table(self, name):
        """
        Create data-tensor for name table
        Parameters
           name: Name of table 
        Returns
           true if create successfully else false
        """
        map_result = Cache.tcache.make_maps(name, False)
        ten_result = Cache.tcache.make_tensor(name)
        return map_result or ten_result
    
    def make_tensor(self):
        """
        Create n-dimensional data tensor for all n dimension tables in cache
        Returns
           true if create successfully else false
        """
        map_result = Cache.tcache.make_maps(False)
        ten_result = Cache.tcache.make_tensor()
        return map_result or ten_result
    
    def make_maps_table(self, name, if_reverse_map):
        """
        Create data-tensor for name table
        Parameters
           name: Name of table 
        Returns
           true if create successfully else false
        """
        return Cache.tcache.make_maps_table(name, if_reverse_map)
    
    def make_maps(self, if_reverse_map):
        """
        Create n-dimensional data tensor for all n dimension tables in cache
        Returns
           true if create successfully else false
        """
        return Cache.tcache.make_maps(if_reverse_map)
    
    def query6(self):
        """
        Run Tpch query 6
        Returns
           query 6 result
        """
        return Cache.tcache.query6()

    def query5(self):
        """
        Run Tpch query 5
        Returns
           query 5 result
        """
        return Cache.tcache.query5()

    def join(self, child_schema_name, child_field_name, parent_schema_name, parent_field_name):
        """
        joints child field with parent field which creates data tensor dimensionality
        Parameters
           child_schema name of child schema
           child_field_name name of child field
           parent_schema TSchema of parent
           parent_field_name name of parent field
        Returns
           True if success else False
        """
        return Cache.tcache.join(child_schema_name, child_field_name, parent_schema_name, parent_field_name)
        
    def slice(self, table_name, offset, length):
        """
        Parameters
          table_name: name of table
          offset: offset from beginning
          length: number for rows to be sliced
        Returns:
          arrow table with the given slice, None if table not found
        """
        return Cache.tcache.slice(table_name, offset, length)

    def read_csv(self, input_file, parse_options, table_name, ttype, schema_name=""):
        """
        read csv file input_file using pyarrow reader. Add it as a table_name in Liten.
        Create arrow table in cache by name
        Parameters
           input_file: Name of input_file to be passed to arrow
           parse_options: parse_options for arrow reader
           table_name: name of table
           ttype: type of table must be DimensionTable or FactTable
           schema_name: if schema already exists
        Returns
           Added table_name
        """
        pa_table = pa.csv.read_csv(input_file=input_file, parse_options=parse_options)
        ttable = Cache.tcache.add_table(table_name, pa_table, ttype, schema_name)
        return table_name
