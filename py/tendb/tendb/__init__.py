from ctypes import cdll
import os

import tendb._tendb as ctendb

basedir = os.path.abspath(os.path.dirname(__file__))
libpath = os.path.join(basedir, 'libtendb.so')
lib = cdll.LoadLibrary(libpath)
        
def new_cache():
    """
    TenDB Cache
    """
    tcache = lib.TCache_GetInstance()
    return tcache

def show_versions():
    """
    TenDB Version
    """
    ver = ctendb.show_versions()
    print(ver)


def add_table(name, table):
    """
    Add Arrow table to ten
    """
    status = ctendb.add_table(name,table)
    return status

def add_cache_table(tcache, name, table):
    """
    Add Arrow table to ten
    """
    status = lib.TCache_AddTable(tcache, name, table)
    return status

def query6(tcache):
    """
    Run Query6
    """
    revenue = lib.Tpch_Query6(tcache)
    return revenue

def query5():
    """
    Run Query5
    """
    return 1.87
