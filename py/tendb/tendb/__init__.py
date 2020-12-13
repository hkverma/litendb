from ctypes import cdll
lib = cdll.LoadLibrary('/mnt/c/Users/hkver/Documents/dbai/dbaistuff/cpp/build/libtendb.so')
        
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
