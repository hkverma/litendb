//
// TenDB Columnar Storage Node
//
// Columnar Data storage
//
// One column chunk stores numComponents of a single column of TenDB
// Arrow in memory, persistent in parquet format
//
#pragma once

#include <vector>
#include <map>

#include <boost/uuid/uuid.hpp>
#include <arrow/api.h>

namespace tendb {

  // Base Array Map
  class TArrayMap {
  public:
    TArrayMap(std::shared_ptr<arrow::Array> arr) : array_(arr) { }
    ~TArrayMap() { }

    static std::shared_ptr<TArrayMap> Make(std::shared_ptr<arrow::Array> arr);

    std::shared_ptr<arrow::Array> array_;

    // TODO needs to be templated
    virtual bool GetMin(int64_t& minVal)
    {
      return false;
    }

    virtual bool GetMax(int64_t& maxVal)
    {
      return false;
    }

    virtual bool GetRowId(int64_t& rowId, int64_t& rowVal)
    {
      return false;
    }

    virtual void GetReverseMap(std::stringstream& ss)
    {
      return ;
    }
  };

  // zone maps for comparable types currently only range type
  // Create a MinMax template class here
  // TODO For now add a hash here, separate it later
  class TInt64ArrayMap : public TArrayMap
  {
  public:
    TInt64ArrayMap(std::shared_ptr<arrow::Array> arr);
    static std::shared_ptr<TInt64ArrayMap> Make(std::shared_ptr<arrow::Array> arr);
    virtual bool GetMin(int64_t& minVal)
    {
      minVal = min_;
      return true;
    }

    virtual bool GetMax(int64_t& maxVal)
    {
      maxVal = max_;
      return true;
    }

    virtual bool GetRowId(int64_t& rowId, int64_t& rowVal);

    virtual void GetReverseMap(std::stringstream& ss);
    
    int64_t min_;
    int64_t max_;
    
    std::map<int64_t, int64_t> rowIds_;
    
  };

  //
  // TODO Use Memory Pool to create an arrow memory pool liks  arrow::MemoryPool* pool_
  //      Use pool to store all the values
  //
  class TColumnMap {
  public:
    TColumnMap(std::shared_ptr<arrow::ChunkedArray> chunkedArray) :
      chunkedArray_(chunkedArray)
    {  }
    ~TColumnMap() { }

    static std::shared_ptr<TColumnMap> Make(std::shared_ptr<arrow::ChunkedArray> chunkedArray);

    std::shared_ptr<arrow::ChunkedArray> chunkedArray_;
    std::vector<std::shared_ptr<TArrayMap>> arrayMap_;

    arrow::Status status_;
  };

};
