"""
Init python in conda release
"""
from ctypes import cdll
import os

import liten as ten

basedir = os.path.abspath(os.path.dirname(__file__))
libpath = os.path.join(basedir, 'libliten.so')
lib = cdll.LoadLibrary(libpath)
        
def show_versions():
    """
    Liten Version
    """
    return ten._liten._version

def new_cache():
    """
    Create a new Liten cache
    """
    tc =  ten._liten.TCache()
    return tc
