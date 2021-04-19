import functools
from functools import wraps

import pyarrow as pa
from pyarrow import csv

import liten._liten as cliten

import sys
import codecs

DimTable = 0
FactTable = 1

def to_bytes(s):
    if type(s) is bytes:
        return s
    elif type(s) is str or (sys.version_info[0] < 3 and type(s) is unicode):
        return codecs.encode(s, 'utf-8')
    else:
        raise TypeError("Expected bytes or string, but got %s." % type(s))
    
class RCLiten:
    """An actor wrapper for Liten Cache"""
    
    num_table = 0
    table_name = ""
    table_type = FactTable
        
    def __init__(self):
        self.tc = cliten.CLiten()
        self.table_name = "_t" + str(self.num_table);
        self.table_type = FactTable
    
    def set_table(self, table_name, table_type):
        self.table_name = table_name
        self.table_type = table_type
    
    def read_csv(self, *args, **kwargs):
        arrow_table = pa.csv.read_csv(*args, **kwargs)
        self.tc.add_table(to_bytes(self.table_name), arrow_table, self.table_type)
        added_table_name = self.table_name
        self.num_table = self.num_table + 1
        self.table_name = "_t" + str(self.num_table)
        self.table_type = FactTable
        return added_table_name

    def info(self):
        return self.tc.info()

    def get_table(self, name):
        return self.tc.get_table(name)

    def make_dtensor_table(self, name):
        return self.tc.make_dtensor_table(name)

    def make_dtensor(self):
        return self.tc.make_dtensor()

    def query6(self):
        return self.tc.query6()
    
    def query5(self):
        return self.tc.query5()    
