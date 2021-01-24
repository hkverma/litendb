from ctypes import cdll
import os

import liten._liten as cliten

basedir = os.path.abspath(os.path.dirname(__file__))
libpath = os.path.join(basedir, 'libliten.so')
lib = cdll.LoadLibrary(libpath)
        
def show_versions():
    """
    Liten Version
    """
    return cliten._version

def new_cache():
    """
    Create new cache
    """
    tc =  cliten.CLiten()
    return tc
