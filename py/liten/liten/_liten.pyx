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

import liten.litenutils as litenutils

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

_version = "0.0.1"

cdef class CLiten:
    """
    Liten Cache Class
    """
    DimTable=0
    FactTable=1
    
    def __cinit__(self):
        """
        Create and initialize Liten Cache
        """
        self.sp_tcache = CTCache.GetInstance()
        self.tcache = self.sp_tcache.get()

    def info(self):
        """
        info()
        return cache information including compute and storage 
        Returns
          string containing cache information
        """
        cdef:
           c_string cache_info
        cache_info = self.tcache.GetInfo()
        return cache_info
    
    def add_table(self, name, table, ttype):
        """
        add_table(name, table, ttype)
           Add arrow table in cache by name
        Parameters
           name: name of table
           table: arrow table to be added in liten cache
           ttype: type og table must be DimTable or FactTable
        Returns
           name of the table added
        """
        cdef:
            shared_ptr[CTable] sp_table
            shared_ptr[CTTable] sp_ttable
            CTTable* p_ttable
            CTTable.TType tc_ttype
        sp_table = pyarrow_unwrap_table(table)
        if ttype != self.DimTable and ttype != self.FactTable:
            print("Error: Table must be DimTable or FactTable")
            return "";        
        tc_ttype = <CTTable.TType>ttype
        sp_ttable = self.tcache.AddTable(litenutils.to_bytes(name), sp_table, tc_ttype)
        p_ttable = sp_ttable.get()
        if (NULL == p_ttable):
            print ("Failed to add table=", name)
            return ""
        print ("Added Table=", name)
        return name

    def get_table(self, name):
        """
        get_table(name)
        get arrow table by name name
        Parameters
          name: name of table
        Returns
          Arrow table of given name
        """        
        cdef:
            shared_ptr[CTTable] sp_ttable
            CTTable* p_ttable
            shared_ptr[CTable] sp_table
        sp_ttable = self.tcache.GetTable(name)
        p_ttable = sp_ttable.get()
        if (NULL == p_ttable):
            print ("Failed to get table=", name)
            return None
        sp_table = p_ttable.GetTable()
        pa_table = pyarrow_wrap_table(sp_table)
        return pa_table
    
    def make_dtensor_table(self, name):
        """
        make_dtensor_table(name)
        Create data-tensor for name table
        Parameters
           name: Name of table 
        Returns
           true if create successfully else false
        """
        result = self.tcache.MakeMaps(name)
        if (result):
            print ("Failed to create data-tensor for ", name)
        return result

    def make_dtensor(self):
        """
        make_dtensor(name)
        Create n-dimensional data tensor for all n dimension tables in cache
        Returns
           true if create successfully else false
        """
        result = self.tcache.MakeMaps()
        if (result):
            print ("Failed to create data-tensor")
        return result
    
    def query6(self):
        """
        query6()
        Run Tpch query 6
        Returns
           query 6 result
        """
        cdef:
            shared_ptr[CTpchDemo] sp_tpch_demo
            CTpchDemo* p_tpch_demo
        sp_tpch_demo = CTpchDemo.GetInstance(self.sp_tcache)
        p_tpch_demo = sp_tpch_demo.get()
        print (""" TPCH QUERY 6 
SELECT 
  SUM(L_EXTENDEDPRICE * L_DISCOUNT) AS REVENUE 
FROM 
  LINEITEM
WHERE
  L_SHIPDATE >= DATE '1997-01-01'
  AND L_SHIPDATE < DATE '1997-01-01' + INTERVAL '1' YEAR
  AND L_DISCOUNT BETWEEN 0.07 - 0.01 AND 0.07 + 0.01
  AND L_QUANTITY < 25;
""")
        result = p_tpch_demo.Query6()
        print("Revenue=",result);
        print("")
        q6di = Source(q6diggraphcmd, filename="_temp.gv", format="png")
        return q6di

    def query5(self):
        """
        query5()
        Run Tpch query 5
        Returns
           query 5 result
        """
        cdef:
            shared_ptr[CTpchDemo] sp_tpch_demo
            CTpchDemo* p_tpch_demo
            shared_ptr[unordered_map[c_string, double]] sp_result
            unordered_map[c_string, double]* p_result
        sp_tpch_demo = CTpchDemo.GetInstance(self.sp_tcache)
        p_tpch_demo = sp_tpch_demo.get()
        print (""" 
SELECT
	N_NAME,
	SUM(L_EXTENDEDPRICE * (1 - L_DISCOUNT)) AS REVENUE
FROM
	CUSTOMER,
	ORDERS,
	LINEITEM,
	SUPPLIER,
	NATION,
	REGION
WHERE
	C_CUSTKEY = O_CUSTKEY
	AND L_ORDERKEY = O_ORDERKEY
	AND L_SUPPKEY = S_SUPPKEY
	AND C_NATIONKEY = S_NATIONKEY
	AND S_NATIONKEY = N_NATIONKEY
	AND N_REGIONKEY = R_REGIONKEY
	AND R_NAME = 'EUROPE'
	AND O_ORDERDATE >= DATE '1995-01-01'
	AND O_ORDERDATE < DATE '1995-01-01' + INTERVAL '1' YEAR
GROUP BY
	N_NAME
ORDER BY
	REVENUE DESC;
""")
        sp_result = p_tpch_demo.Query5()
        p_result = sp_result.get()
        q5result = { }        
        if (NULL == p_result):
            print("Failed to run Query5")
            return q5result
        cdef unordered_map[c_string, double].iterator it = p_result.begin()
        while (it != p_result.end()):
            key = deref(it).first
            value = deref(it).second
            q5result[key] = value
            print(key,"=",value)
            postincrement(it)
        print("")
        q5di = Source(q5diggraphcmd, filename="_temp.gv", format="png")
        return q5di

    def show_versions(self):
        """
        show_versions()
        Returns
          Liten cache version
        """
        return _version
    
    @property
    def version(self):
        """ Liten Cache Version """
        return _version
