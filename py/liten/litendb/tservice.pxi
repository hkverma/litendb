# cython: profile=False
# distutils: language = c++
# cython: embedsignature = True
# cython: language_level = 3
"""
Cache System
"""
from cython.operator cimport dereference as deref, postincrement
from pyarrow.includes.libarrow cimport *
from pyarrow.lib cimport *
from .includes.ctcache cimport *

import sys
import codecs
import time

cdef class TService:
    """
    Liten Service Class
    """
    def __cinit__(self):
        """
        Create and initialize Liten Service
        """
        self.sp_tservice = CTService.GetInstance()
        self.tservice = self.sp_tservice.get()

    def start(self):
        self.tservice.Start()
        return

    def shutdown(self):
        self.tservice.Shutdown()
        return
