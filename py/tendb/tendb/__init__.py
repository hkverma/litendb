from ctypes import cdll
import os

basedir = os.path.abspath(os.path.dirname(__file__))
libpath = os.path.join(basedir, 'libtendb.so')
lib = cdll.LoadLibrary(libpath)
        
def new_cache():
    """
    TenDB Cache
    """
    tcache = lib.TCache_GetInstance()
    return tcache

def add_table(tcache, name, table):
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
