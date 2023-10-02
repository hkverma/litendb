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
from .includes.ctcache cimport *

from graphviz import Digraph
from graphviz import Source

import sys
import codecs

import utils
import lib as cliten

# should be same as ../setup.py::setup::version
_version = "0.0.11"

# Schema
include "tschema.pxi"

# Table
include "ttable.pxi"

# Cache
include "tcache.pxi"

# Service
include "tservice.pxi"
