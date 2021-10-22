"""
Init python in conda release
"""
from ctypes import cdll
import os

import liten
import liten.lib as cliten
from liten.lib import *

from liten import utils

from liten.cache import Cache
from liten.schema import Schema
from liten.table import Table
from liten.service import Service

basedir = os.path.abspath(os.path.dirname(__file__))
libpath = os.path.join(basedir, 'libliten.so')
lib = cdll.LoadLibrary(libpath)

liten_service = Service()

def show_versions():
    """
    Liten Version
    """
    return cliten._version

def start():
    liten_service.start()
    return

def shutdown():
    liten_service.shutdown()
    return

def cache():
    """
    Create a new Liten cache
    """
    tc =  liten.Cache()
    return tc
