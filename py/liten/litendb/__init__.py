"""
Init python in conda release
"""
from ctypes import cdll
import os

import lib as cliten
from .lib import *

import utils

from .cache import Cache
from .schema import Schema
from .table import Table
from .service import Service

basedir = os.path.abspath(os.path.dirname(__file__))
libpath = os.path.join(basedir, 'libliten.so')
lib = cdll.LoadLibrary(libpath)

def show_versions():
    """
    Liten Version
    """
    return cliten._version
