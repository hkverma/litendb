# cython: profile=False
# distutils: language = c++
# cython: embedsignature = True
# cython: language_level = 3

from cython.operator cimport dereference as deref, postincrement
from pyarrow.includes.libarrow cimport *
from pyarrow.lib cimport *
from tendb.includes.dtensor cimport *

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
        return result

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
        print("Result Size=", p_result.size())
        cdef unordered_map[c_string, double].iterator it = p_result.begin()
        while (it != p_result.end()):
            key = deref(it).first
            value = deref(it).second
            q5result[key] = value
            print(key,"=",value)
            postincrement(it)
        return q5result
    
    @property
    def version(self):
        return "0.0.2"
