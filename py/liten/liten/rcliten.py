"""
A remote actor for Liten Cache
"""
import functools
from functools import wraps

import pyarrow as pa
from pyarrow import csv

import liten._liten as cliten
import liten.utils as litenutils

import sys
import codecs

FactTable = 1

class RCLiten:
    """
    A remote actor wrapper for Liten Cache
    """
    
    num_table = 0
    table_name = ""
    table_type = FactTable
        
    def __init__(self):
        """
        Create and initialize remote Liten Cache
        """
        self.tc = cliten.CLiten()
        self.table_name = "_t" + str(self.num_table);
        self.table_type = FactTable
        
    def __cinit__(self):
        """
        Create and initialize remote Liten Cache
        """
        self.tc = cliten.CLiten()
        self.table_name = "_t" + str(self.num_table);
        self.table_type = FactTable
    
    def set_table(self, table_name, table_type):
        """
        set_table(table_name, table_type):
        Set table_name and its table_type as DimTable or FactTabls
        Parameters
          table_name: table name to be used
          table_type: set table type to be FactTable or DimTable
        Returns
          None
        """
        self.table_name = table_name
        self.table_type = table_type
    
    def read_csv(self, *args, **kwargs):
        """
        read_csv(*args, **kwargs)
        read csv file with the table set by set_table. It table_name is not set it picks a table
        name like _t0 where 0 is a number uniqifying table names
        It uses pyarrow.csv.read_csv and passes arguments to this function
        Parameters
          arguments are passed to pyarrow.csv.read_csv You may import pyarrow as pa and look at
          pa.csv.read_csv doc to understand the parameters to pass
        Returns
          table_name that has been added
        """
        arrow_table = pa.csv.read_csv(*args, **kwargs)
        self.tc.add_table(litenutils.to_bytes(self.table_name), arrow_table, self.table_type)
        added_table_name = self.table_name
        self.num_table = self.num_table + 1
        self.table_name = "_t" + str(self.num_table)
        self.table_type = FactTable
        return added_table_name

    def info(self):
        """
        info()
        return cache information including compute and storage 
        Returns
          string containing cache information
        """
        return self.tc.info()

    def get_table(self, name):
        """
        get_table(name)
        get arrow table by name name
        Parameters
          name: name of table
        Returns
          Arrow table of given name
        """
        return self.tc.get_table(name)

    def make_dtensor_table(self, name):
        """
        make_dtensor_table(name)
        Create data-tensor for name table
        Parameters
           name: Name of table 
        Returns
           true if create successfully else false
        """
        return self.tc.make_dtensor_table(name)

    def make_dtensor(self):
        """
        make_dtensor(name)
        Create n-dimensional data tensor for all n dimension tables in cache
        Returns
           true if create successfully else false
        """
        return self.tc.make_dtensor()

    def query6(self):
        """
        query6()
        Run Tpch query 6
        Returns
           query 6 result
        """
        return self.tc.query6()
    
    def query5(self):
        """
        query5()
        Run Tpch query 5
        Returns
           query 5 result
        """
        return self.tc.query5()    
