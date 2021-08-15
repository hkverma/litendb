"""
Init python in conda release
"""
from ctypes import cdll
import os

import liten as ten
from liten import tcache
from liten import ttable
from liten import tschema

basedir = os.path.abspath(os.path.dirname(__file__))
libpath = os.path.join(basedir, 'libliten.so')
lib = cdll.LoadLibrary(libpath)
        
def show_versions():
    """
    Liten Version
    """
    return ten.tcache._version

def new_cache():
    """
    Create a new Liten cache
    """
    tc =  ten.tcache.TCache()
    return tc
