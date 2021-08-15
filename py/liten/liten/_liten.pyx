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
from liten.includes.dtensor cimport *

from graphviz import Digraph
from graphviz import Source

import sys
import codecs

import liten as ten
from liten import tcache
from liten import ttable
from liten import tschema

def q6digraph():
    """
    Graph diagram for Query6 plan
    """
    q6 = Digraph(comment='Tpch Query6')
    q6.graph_attr['rankdir'] = 'LR'
    q6.graph_attr['bgcolor'] = 'whitesmoke'
    q6.edge_attr.update(arrowhead='vee',arrowsize='1')
    q6.node_attr.update(shape='rect',fontname='Arial', fontcolor='blue', fontsize='12')
    q6.node('A','Scan\n(lineitem)')
    q6.node('B','Filter\n(lineitem)')
    q6.node('C','Aggregate\n(lineitem)')
    q6.edges(['AB','BC'])
    return q6

def q5digraph():
    """
    Graph diagram for Query5 plan
    """
    q5 = Digraph(comment='Tpch Query5')
    q5.graph_attr['rankdir'] = 'LR'
    q5.graph_attr['bgcolor'] = 'whitesmoke'
    q5.edge_attr.update(arrowhead='vee',arrowsize='1')
    q5.node_attr.update(shape='rect',fontname='Arial', fontcolor='blue', fontsize='11')
    q5.node('A','Scan\n(lineitem)')
    q5.node('B','OrdersCell\n[lineitem]')
    q5.node('C','Filter\n(orders)')
    q5.node('D','RegionCell\n[lineitem][supplier][nation][region]')
    q5.node('E','GroupBy Aggr\n(lineitem)')
    q5.edges(['AB','BC','CD','DE'])

q6diggraphcmd = """
digraph Q6{
  rankdir=LR; bgcolor=whitesmoke;
  {
    edge [arrowhead=vee, arrowsize=1]
    node [shape=rect, fontname=Arial, fontcolor=blue, fontsize=12]
    0 [label="Scan\n(lineitem)"]
    1 [label="Filter\n(lineitem)"]
    2 [label="Aggregate\n(lineitem)"]
    0 -> 1
    1 -> 2
  }
}
"""

q5diggraphcmd = """
digraph Q5{
  rankdir=LR; bgcolor=whitesmoke;
  {
    edge [arrowhead=vee, arrowsize=1]
    node [shape=rect, fontname=Arial, fontcolor=blue, fontsize=12]
    0 [label="Scan\n(lineitem)"]
    1 [label="DTensor\n[l_orderkey]"]
    2 [label="Filter\n(orders)"]
    3 [label="DTensor\n[l_suppkey][s_nationkey][n_regionkey]"]
    4 [label="Filter\n(region)"]
    5 [label="Aggregate\n(lineitem)"]
    0 -> 1
    1 -> 2
    2 -> 3
    3 -> 4
    4 -> 5
  }
}
"""

_version = "0.0.2"

cdef class TCache:
    """
    Liten Cache Class
    """
    nameToTSchema = { }
    nameToTTable = { }
    
    def __cinit__(self):
        """
        Create and initialize Liten Cache
        """
        self.sp_tcache = CTCache.GetInstance()
        self.tcache = self.sp_tcache.get()

    def info(self):
        """
        return cache information including compute and storage 
        Returns
          string containing cache information
        """
        cdef:
           c_string cache_info
        cache_info = self.tcache.GetInfo()
        return cache_info

    def add_schema(self, name, ttype, pa_schema):
        """
        Add arrow table in cache by name
        Parameters
           name: name of schema
           ttype: type of table must be DimTable or FactTable
           schema: arrow schema to be added in liten cache
        Returns
           Newly added TSchema or None if failed to add
        """
        cdef:
           shared_ptr[CSchema] sp_schema
           CTResultCTSchema sp_tschema_result
           shared_ptr[CTSchema] sp_tschema
           CTSchema* p_tschema
           TableType tc_ttype

        sp_pa_schema = pyarrow_unwrap_schema(pa_schema)
        if ttype != ten.TTable.Dimension and ttype != ten.TTable.Fact:
            raise TypeError("Type ttype must be Dimension or Fact")
        tc_ttype = <TableType>ttype
        sp_tschema_result = self.tcache.AddSchema(ten.litenutils.to_bytes(name), ttype, sp_pa_schema)
        if (not sp_tschema_result.ok()):
            print(f"Failed to add schema {name}. {sp_tschema_result.status().message()}")
            return None
        sp_tschema = sp_tschema_result.ValueOrDie()
        p_tschema = sp_tschema.get()
        if (NULL == p_tschema):
            print (f"Failed to add schema {name}")
            return None

        tschema = TSchema()
        tschema.sp_tschema = sp_tschema
        tschema.p_tschema = p_tschema
        tschema.sp_pa_schema = sp_pa_schema
        tschema.ttype = ttype
        
        self.nameToTSchema[name] = tschema
        return tschema
    
    def get_schema(self, name):
        tschema = self.nameToTSchema[name]
        return tschema

    def add_table(self, name, pa_table, ttype, schema_name=""):
        """
        Create arrow table in cache by name
        Parameters
           name: name of table
           table: arrow table to be added in liten cache
           ttype: type of table must be DimTable or FactTable
        Returns
           Added Liten TTable
        """
        cdef:
           shared_ptr[CTable] sp_pa_table
           CTResultCTTable sp_ttable_result
           shared_ptr[CTTable] sp_ttable
           CTTable* p_ttable
           TableType tc_ttype 
        
        sp_pa_table = pyarrow_unwrap_table(pa_table)
        if ttype != TTable.Dimension and ttype != TTable.Fact:
            print(f"Table type must be Dimension or Fact")
            return None
        
        tc_ttype = <TableType>ttype
        sp_ttable_result = self.tcache.AddTable(ten.litenutils.to_bytes(name), tc_ttype, sp_pa_table, schema_name)
        if (not sp_ttable_result.ok()):
            print (f"Failed to add table {name} {sp_ttable_result.status().message()}")
            return None
        
        sp_ttable = sp_ttable_result.ValueOrDie()
        p_ttable = sp_ttable.get()
        if (NULL == p_ttable):
            print (f"Failed to build table {name}")
            return None

        ttable = TTable()
        ttable.sp_pa_table = sp_pa_table
        ttable.sp_ttable = sp_ttable
        ttable.p_ttable = p_ttable
        ttable.ttype = ttype
        
        self.nameToTTable[name] = ttable
        return ttable

    def get_table(self, name):
        table = self.nameToTTable[name]
        return table
    

cdef class TSchema:
    """
    Liten Schema Class
    """
    ttype = ten.TTable.Fact
    
    def __cinit__(self):
        ttype = ten.TTable.Fact
        
    def get_pyarrow_schema(self):
        """
        Get pyarrow schema from Liten schema
        """
        pa_schema = pyarrow_wrap_schema(self.sp_pa_schema)
        return pa_schema
    
    def get_name(self):
        """
        Returns
          unique name of the table
        """
        name = self.sp_tschema.get().GetName()
        return ten.litenutils.to_bytes(name)

    def get_info(self):
        """
        Returns
          unique name of the table
        """
        schema_str = self.p_tschema.ToString()
        return ten.litenutils.to_bytes(schema_str)
    
    def get_type(self):
        """
        Returns
          Dimension or Fact Table
        """
        return self.ttype

    def join(self, field_name, parent_schema, parent_field_name):
        if (not type(parent_schema) is TSchema):
            print(f"parent_schema {type(parent_schema)} must be TSchema")
            return False
        p_parent_schema = <TSchema>parent_schema
        status = self.p_tschema.Join(field_name, p_parent_schema.sp_tschema, parent_field_name)
        if (status.ok()):
            return True
        else:
            print(status.message())
            return False

cdef class TTable:
    """
    Liten Table Class
    """
    
    Dimension = 0
    Fact = 1
    ttype = Fact
    
    def __cinit__(self):
        TTable.Dimension = 0
        TTable.Fact = 1
        
    def get_pyarrow_table(self):
        """
        Returns
          Arrow table
        """
        pa_table = pyarrow_wrap_table(self.sp_pa_table)
        return pa_table

    def get_name(self):
        """
        Returns
          unique name of the table
        """
        name = self.p_ttable.GetName()
        return ten.litenutils.to_bytes(name)
    
    def get_type(self):
        """
        Returns
          Dimension or Fact Table
        """
        return self.ttype
