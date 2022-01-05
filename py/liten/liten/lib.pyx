# cython: profile=False
# distutils: language = c++
# cython: embedsignature = True
# cython: language_level = 3
"""
CLiten Cache System
"""
from cython.operator cimport dereference as deref, postincrement
from pyarrow.includes.libarrow cimport *
from pyarrow.lib cimport *
from liten.includes.ctcache cimport *

from graphviz import Digraph
from graphviz import Source

import sys
import codecs

import liten
from liten import utils
import liten.lib as cliten

# should be same as ../setup.py::setup::version
_version = "0.0.5"

# Schema
include "tschema.pxi"

# Table
include "ttable.pxi"

# Cache
include "tcache.pxi"

# Service
include "tservice.pxi"
