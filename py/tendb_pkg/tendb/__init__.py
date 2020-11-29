from ctypes import cdll
lib = cdll.LoadLibrary('/mnt/c/Users/hkver/Documents/dbai/dbaistuff/cpp/build/libtendb.so')

class Foo(object):
    def __init__(self):
        self.obj = lib.Foo_new()

    def bar(self):
        lib.Foo_bar(self.obj)
        
def init():
    """
    Initialize TenDB Stack
    """
    return "Init"

def add_table():
    """
    Add Arrow table to ten
    """
    return "AddTable"

def query6():
    """
    Run Query6
    """
    return "Query6"

def query5():
    """
    Run Query5
    """
    return "Query5"

def test():
    """
    Test
    """
    f = Foo()
    return f.bar()
