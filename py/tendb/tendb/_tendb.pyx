# cython: profile=False
# distutils: language = c++
# cython: embedsignature = True
# cython: language_level = 3

from cython.operator cimport dereference as deref, postincrement
from pyarrow.includes.libarrow cimport *
from pyarrow.lib cimport *
from tendb.includes.dtensor cimport *

from graphviz import Digraph
from graphviz import Source

def q6digraph():
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
    q5 = Digraph(comment='Tpch Query5')
    q5.graph_attr['rankdir'] = 'LR'
    q5.graph_attr['bgcolor'] = 'whitesmoke'
    q5.edge_attr.update(arrowhead='vee',arrowsize='1')
    q5.node_attr.update(shape='rect',fontname='Arial', fontcolor='blue', fontsize='11')
    q5.node('A','Scan\n(lineitem)')
    q5.node('B','Join\n(lineitem,orders)')
    q5.node('C','Filter\n(orders)')
    q5.node('D','Join\n(lineitem,supplier)')
    q5.node('E','Join\n(supplier,nation)')
    q5.node('F','Join\n(nation,region)')
    q5.node('G','GroupBy Aggr\n(lineitem)')
    q5.edges(['AB','BC','CD','DE','EF','FG'])

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
    1 [label="Join\n(lineitem,orders)"]
    2 [label="Filter\n(orders)"]
    3 [label="Join\n(lineitem,supplier)"]
    4 [label="Join\n(supplier,nation)"]
    5 [label="Join\n(nation,region)"]
    6 [label="GroupBy Aggr\n(lineitem)"]
    0 -> 1
    1 -> 2
    2 -> 3
    3 -> 4
    4 -> 5
    5 -> 6
  }
}
"""

cdef class CTenDB:
    def __cinit__(self):
        self.tcache = NULL

    def __init__(self):
        self.tcache = NULL
        self.sp_tcache = CTCache.GetInstance()
        self.tcache = self.sp_tcache.get()
        print("Added a new cache")

    def show_versions(self):
        return "0.0.2"

    def info(self):
        cdef:
           c_string cache_info
        cache_info = self.tcache.GetInfo()
        return cache_info
    
    def add_table(self, name, table):
        cdef:
            shared_ptr[CTable] sp_table
            shared_ptr[CTTable] sp_ttable
            CTTable* p_ttable
        sp_table = pyarrow_unwrap_table(table)
        sp_ttable = self.tcache.AddTable(name, sp_table)
        p_ttable = sp_ttable.get()
        if (NULL == p_ttable):
            print ("Failed to add table=", name)
            return ""
        print ("Added Table=", name)
        return name

    def make_dtensor(self, name):
        result = self.tcache.MakeMaps(name)
        if (result):
            print ("Failed to create data-tensor for ", name)
        return result

    def query6(self):
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
    
    @property
    def version(self):
        return "0.0.2"
