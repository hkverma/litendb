"""
Liten Cache
"""
import liten
from liten import utils
import liten.lib as cliten

from liten.schema import Schema
from liten.table import Table

import sys
import codecs

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
    def DerivedFeatureField(self):
        return Cache.tcache.DerivedFeatureField

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

    def schema_info(self):
        """
        return cache information including compute and storage 
        Returns
          string containing cache information
        """
        return Cache.tcache.schema_info()
    
    def add_schema(self, name, ttype, pa_schema):
        """
        Add arrow table in cache by name
        Parameters
           name: name of schema
           ttype: type of table must be DimensionTable or FactTable
           schema: arrow schema to be added in liten cache
        Returns
           Liten Schema or ValueError if failed to add
        """
        return Schema(Cache.tcache.add_schema(name, ttype, pa_schema))
    
    def get_schema(self, name):
        """
        Get schema by bame
        Parameters
           name: name of schema
        Returns
           Liten Schema if exists else None
        """
        return Schema(Cache.tcache.get_schema(name))

    def add_schema_from_ttable(self, table):
        """
        Add schema associated with ttable by name. If a schema exists by name, that is returned
        Parameters
           ttable: schema in liten table ttable
        Returns
           Liten schema TSchema
        """
        return Schema(Cache.tcache.add_schema_from_ttable(table.ttable))

    def add_table(self, name, pa_table, ttype, schema_name=""):
        """
        Create arrow table in cache by name
        Parameters
           name: name of table
           table: arrow table to be added in liten cache
           ttype: type of table must be DimensionTable or FactTable
        Returns
           Added Liten TTable
        """
        return Table(Cache.tcache.add_table(name, pa_table, ttype, schema_name))

    def get_table(self, name):
        """
        Get table by name
        Parameters
           name: name of table
        Returns
           Liten table TTable if exists else None
        """
        return Table(Cache.tcache.get_table(name))
        
    def make_tensor_table(self, name):
        """
        Create data-tensor for name table
        Parameters
           name: Name of table 
        Returns
           true if create successfully else false
        """
        return Cache.tcache.make_tensor_table(name)
    
    def make_tensor(self):
        """
        Create n-dimensional data tensor for all n dimension tables in cache
        Returns
           true if create successfully else false
        """
        return Cache.tcache.make_tensor()
    
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
