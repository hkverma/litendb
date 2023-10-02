# disutils: language = c++
# cython: language_level = 3
"""
Cython interface class to C++ Liten Cache and tables
"""
from libc.stdint cimport *
from libcpp cimport bool as c_bool, nullptr
from libcpp.functional cimport function
from libcpp.memory cimport shared_ptr, unique_ptr, make_shared
from libcpp.string cimport string as c_string
from libcpp.utility cimport pair
from libcpp.vector cimport vector
from libcpp.unordered_map cimport unordered_map
from libcpp.unordered_set cimport unordered_set
from libcpp cimport bool

from pyarrow.includes.libarrow cimport *
from pyarrow.includes.libarrow cimport (CArray, CBuffer, CDataType,
                                        CField, CRecordBatch, CSchema,
                                        CTable, CTensor, CSparseCOOTensor,
                                        CSparseCSRMatrix, CSparseCSCMatrix,
                                        CSparseCSFTensor)

cdef extern from "common.h" namespace "liten" nogil:

  cdef cppclass CTStatus" liten::TStatus":
    bool ok() const
    c_string message() const
      
  cdef cppclass CTResultCTRowBlock" liten::TResult<std::shared_ptr<liten::TRowBlock>>":
     bool ok() const
     const shared_ptr[CTRowBlock]& ValueOrDie() const
     const CTStatus& status() const

  cdef cppclass CTResultCTTable" liten::TResult<std::shared_ptr<liten::TTable>>":
     bool ok() const
     const shared_ptr[CTTable]& ValueOrDie() const
     const CTStatus& status() const
     
  cdef cppclass CTResultCTSchema" liten::TResult<std::shared_ptr<liten::TSchema>>":
     bool ok() const
     const shared_ptr[CTSchema]& ValueOrDie() const
     const CTStatus& status() const

  cdef cppclass CTResultFieldType" liten::TResult<liten::FieldType>":
     bool ok() const
     const FieldType& ValueOrDie() const
     const CTStatus& status() const
     
cdef extern from "cache.h" namespace "liten" nogil:

   ctypedef enum TableType: DimensionTable, FactTable

   ctypedef enum FieldType: DimensionField, MetricField, FeatureField, EmbeddingField

# CTRowBlock is liten::TRowBlock in Cython
   cdef cppclass CTRowBlock" liten::TRowBlock":
      int NumColumns()
      int NumRows()
   
# CTTable is liten::TTable in Cython. CTable is arrow::Table cython from pyarrow.
   cdef cppclass CTTable" liten::TTable":
      c_string GetName()
      shared_ptr[CTable] GetTable()
      shared_ptr[CTSchema] GetSchema()
      TableType GetType()
      shared_ptr[CTable] Slice(int64_t offset, int64_t length)
      CTStatus AddArrowTable(shared_ptr[CTable] table)
      CTResultCTRowBlock AddRowBlock(shared_ptr[CRecordBatch] rb)

# CTSchema is liten::TSchema in Cython. CSchema is arrow::Schema cython from pyarrow.
   cdef cppclass CTSchema" liten::TSchema":
      c_string GetName()
      c_string ToString()
      TableType GetType()
      shared_ptr[CSchema] GetSchema()
      CTStatus Join(c_string fieldName, shared_ptr[CTSchema] parentSchema, c_string parentFieldName)
      CTResultFieldType GetFieldType(c_string fieldName)
      CTStatus SetFieldType(c_string fieldName, FieldType fieldType)
      
# CTCache is liten::TCache      
   cdef cppclass CTCache" liten::TCache":
      @staticmethod
      shared_ptr[CTCache] GetInstance()
      c_string GetInfo()
      c_string GetComputeInfo()
      c_string GetTableInfo()
      c_string GetSchemaInfo()      
      
      CTResultCTTable AddTable(c_string tableName, TableType type, c_string schemaName)
      shared_ptr[CTTable] GetTable(c_string name) const

      
      CTResultCTSchema AddSchema(c_string schemaName, TableType type, shared_ptr[CSchema] schema)
      shared_ptr[CTSchema] GetSchema(c_string name) const
      
      CTStatus MakeMaps(c_string name, bool if_reverse_map)
      CTStatus MakeMaps(bool if_reverse_map)
      
      CTStatus MakeTensor(c_string name)
      CTStatus MakeTensor()
      
      shared_ptr[CTable] Slice(c_string tableName, int64_t offset, int64_t length)

# CTService is liten::TService
   cdef cppclass CTService" liten::TService":
      @staticmethod
      shared_ptr[CTService] GetInstance()
      void Start()
      void Shutdown()
      

cdef extern from "TpchDemo.h" namespace "liten" nogil:
   cdef cppclass CTpchDemo" liten::TpchDemo":
       @staticmethod
       shared_ptr[CTpchDemo] GetInstance(shared_ptr[CTCache] tCache)
       double Query6()
       shared_ptr[unordered_map[c_string, double]] Query5(bool use_tensor)
