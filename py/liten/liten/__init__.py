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

from liten.session import Session

basedir = os.path.abspath(os.path.dirname(__file__))
libpath = os.path.join(basedir, 'libliten.so')
lib = cdll.LoadLibrary(libpath)

def show_versions():
    """
    Liten Version
    """
    return cliten._version
