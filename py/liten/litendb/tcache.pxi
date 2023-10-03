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

from graphviz import Digraph
from graphviz import Source

import sys
import codecs
import time
from .utils import to_bytes

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
    1 [label="Tensor\n[l_orderkey]"]
    2 [label="Filter\n(orders)"]
    3 [label="Tensor\n[l_suppkey][s_nationkey][n_regionkey]"]
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

    @property
    def FactTable(self):
        return 1

    @property
    def DimensionTable(self):
        return 0
    
    @property
    def DimensionField(self):
        return 0

    @property
    def MetricField(self):
        return 1

    @property
    def FeatureField(self):
        return 2

    @property
    def EmbeddingField(self):
        return 3

    def info(self):
        cdef:
           c_string cache_info
        cache_info = self.tcache.GetInfo()
        return cache_info

    def compute_info(self):
        cdef:
           c_string info
        info = self.tcache.GetComputeInfo()
        return info

    def table_info(self):
        cdef:
           c_string info
        info = self.tcache.GetTableInfo()
        return info

    def get_table_exc(self, name):
        table_name = to_bytes(name)
        if table_name in self.nameToTTable:
            ttable = self.nameToTTable[table_name]
            return ttable
        raise ValueError(f"No existing schema {name} in cache.")
    
    def get_table_pyarrow(self, name):
        ttable = self.get_table_exc(name)
        return ttable.get_pyarrow_table()
    
    def get_table_type(self, name):
        ttable = self.get_table_exc(name)
        return ttable.get_type()

    def schema_info(self):
        cdef:
           c_string info
        info = self.tcache.GetSchemaInfo()
        return info

    def get_schema_exc(self, name):
        schema_name = to_bytes(name)
        if schema_name in self.nameToTSchema:
            tschema = self.nameToTSchema[schema_name]
            return tschema
        raise ValueError(f"No existing schema {name} in cache.")
    
    def get_schema_info(self, name):
        tschema = self.get_schema_exc(name)
        return tschema.get_info()            

    def get_schema_pyarrow(self, name):
        tschema = self.get_schema_exc(name)
        return tschema.get_pyarrow_schema()
    
    def get_schema_type(self, name):
        tschema = self.get_schema_exc(name)
        return tschema.get_type()

    def get_schema_field_type(self, name, field_name):
        tschema = self.get_schema_exc(name)
        return tschema.get_field_type(field_name)

    def add_schema(self, name, ttype, pa_schema):
        cdef:
           shared_ptr[CSchema] sp_schema
           CTResultCTSchema sp_tschema_result
           shared_ptr[CTSchema] sp_tschema
           CTSchema* p_tschema
           TableType tc_ttype

        # TBD Check if the name has the same pa_schema
        if name in self.nameToTSchema:
            print(f"Found already existing schema {name} in cache.")
            return name
        
        sp_pa_schema = pyarrow_unwrap_schema(pa_schema)
        if ttype != self.DimensionTable and ttype != self.FactTable:
            raise TypeError("Type ttype must be Dimension or Fact")
        tc_ttype = <TableType>ttype
        sp_tschema_result = self.tcache.AddSchema(to_bytes(name), ttype, sp_pa_schema)
        if (not sp_tschema_result.ok()):
            raise TypeError(f"Failed to add schema {name}. {sp_tschema_result.status().message()}")

        sp_tschema = sp_tschema_result.ValueOrDie()
        p_tschema = sp_tschema.get()
        if (NULL == p_tschema):
            raise TypeError (f"Failed to add schema {name}")

        tschema = TSchema()
        tschema.sp_tschema = sp_tschema
        tschema.p_tschema = p_tschema
        tschema.sp_pa_schema = sp_pa_schema
        tschema.tcache = self
        
        self.nameToTSchema[name] = tschema
        return name
    
    def set_schema_field_type(self, schema_name, field_name, field_type):
        tschema = self.get_schema(schema_name)
        if (None == tschema):
            raise ValueError(f"Invalid schema name {schema_name}")
        
        field_names = []
        field_types = []
        if (list == type(field_name)):
            field_names = field_name
        else:
            field_names = [field_name]
        if (list != field_type):
            field_types = [field_type] * len(field_names)
            
        if (len(field_names) != len(field_types)):
            raise ValueError(f"Incorrect size for field names {len(field_names)} and types {len(field_types)}")

        for i in range(0, len(field_names)):
            if (not tschema.set_field_type(field_names[i], field_types[i])):
                raise ValueError(f"Failed to set field name {field_names[i]} to type {field_types[i]} for schema {schema_name}")
            
        return True

    def if_valid_schema(self, name):
        schema_name = to_bytes(name)
        if schema_name in self.nameToTSchema:
            tschema = self.nameToTSchema[schema_name]
            return True
        return False

    def get_schema(self, name):
        schema_name = to_bytes(name)
        if schema_name in self.nameToTSchema:
            tschema = self.nameToTSchema[schema_name]
            return tschema
        return None
    
    def add_schema_from_ttable(self, ttable):
        cdef:
           shared_ptr[CTSchema] sp_tschema
           CTSchema* p_tschema

        p_ttable = <TTable>ttable
        sp_tschema = p_ttable.p_ttable.GetSchema()
        p_tschema = sp_tschema.get()

        schema_name = to_bytes(p_tschema.GetName())
        if schema_name in self.nameToTSchema:
            print(f"Found already existing schema {schema_name} in cache.")            
            return schema_name
        
        tschema = TSchema()
        tschema.sp_tschema = sp_tschema
        tschema.p_tschema = p_tschema
        tschema.sp_pa_schema = p_tschema.GetSchema()
        tschema.tcache = self
        
        self.nameToTSchema[schema_name] = tschema
        return schema_name        
    
    def add_table(self, name, pa_table, ttype, schema_name=""):
        cdef:
           shared_ptr[CTable] sp_pa_table
           CTResultCTTable sp_ttable_result
           shared_ptr[CTTable] sp_ttable
           CTTable* p_ttable
           TableType tc_ttype 
        
        sp_pa_table = pyarrow_unwrap_table(pa_table)
        if ttype != self.DimensionTable and ttype != self.FactTable:
            raise ValueError(f"Table type must be Dimension or Fact")
        
        tc_ttype = <TableType>ttype
        sp_ttable_result = self.tcache.AddTable(to_bytes(name), tc_ttype, to_bytes(schema_name))
        if (not sp_ttable_result.ok()):
            raise ValueError(f"Failed to add table {name} {sp_ttable_result.status().message()}")
        
        sp_ttable = sp_ttable_result.ValueOrDie()
        p_ttable = sp_ttable.get()
        if (NULL == p_ttable):
            raise ValueError(f"Failed to build table {name}")

        arr_status = p_ttable.AddArrowTable(sp_pa_table)
        if (not arr_status.ok()):
            raise ValueError(f"Failed to add table {name} {arr_status.message()}")

        ttable = TTable()
        ttable.sp_pa_table = sp_pa_table
        ttable.sp_ttable = sp_ttable
        ttable.p_ttable = p_ttable
        ttable.tcache = self

        self.nameToTTable[name] = ttable
        self.add_schema_from_ttable(ttable)        
        return name

    def if_valid_table(self, name):
        table_name = to_bytes(name)
        if table_name in self.nameToTTable:
            return True
        return False
        
    def make_maps_table(self, name, bool if_reverse_map):
        result = self.tcache.MakeMaps(name, if_reverse_map)
        if (not result.ok()):
            print ("Failed to create data-tensor for ", name)
        return result.ok()

    def make_maps(self, bool if_reverse_map):
        result = self.tcache.MakeMaps(if_reverse_map)
        if (not result.ok()):
            print ("Failed to create data-tensor")
        return result.ok()
    
    def make_tensor_table(self, name):
        result = self.tcache.MakeTensor(name)
        if (not result.ok()):
            print ("Failed to create data-tensor for ", name, " msg=", result.message())
        return result.ok()

    def make_tensor(self):
        result = self.tcache.MakeTensor()
        if (not result.ok()):
            print ("Failed to create data-tensor with msg=", result.message())
        return result.ok()
    
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
        start = time.time_ns();
        result = p_tpch_demo.Query6()
        end = time.time_ns();
        print("Revenue=",result);
        print("")
        print("Time(ms)=",(end-start)/1000000)
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
        start = time.time_ns();
        sp_result = p_tpch_demo.Query5(True)
        end = time.time_ns();
        p_result = sp_result.get()
        q5result = { }        
        if (NULL == p_result):
            print("Failed to run Query5")
            return q5result
        print("Revenue=")
        cdef unordered_map[c_string, double].iterator it = p_result.begin()
        while (it != p_result.end()):
            key = deref(it).first
            value = deref(it).second
            q5result[key] = value
            print(key,"=",value)
            postincrement(it)
        print("")
        print("Time(ms)=",(end-start)/1000000)
        print("")
        q5di = Source(q5diggraphcmd, filename="_temp.gv", format="png")
        return q5di

    def join(self, child_schema_name, child_field_name, parent_schema_name, parent_field_name):
        child_schema = self.get_schema(child_schema_name)
        if (None == child_schema):
            return False
        parent_schema = self.get_schema(parent_schema_name)
        if (None == parent_schema):
            return False
        result = child_schema.join(child_field_name, parent_schema, parent_field_name)
        return result
        
    def slice(self, table_name, offset, length):
        """
        Parameters
          table_name: name of table
          offset: offset from beginning
          length: number for rows to be sliced
        Returns:
          arrow table with the given slice, None if table not found
        """
        sp_table = self.tcache.Slice(to_bytes(table_name), offset, length)
        if (NULL == sp_table.get()):
            print ("Failed to get table=", table_name)
            return None
        arr_table = pyarrow_wrap_table(sp_table)
        return arr_table
