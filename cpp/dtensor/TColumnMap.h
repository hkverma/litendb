//
// TenDB Columnar Storage Node
//
// Columnar Data storage
//
// One column chunk stores numComponents of a single column of TenDB
// Arrow in memory, persistent in parquet format
//
#pragma once

#include <boost/uuid/uuid.hpp>
#include <arrow/api.h>
#include <vector>

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
    
    
  };

  // zone maps for comparable types currently only range type
  // Create a MinMax template class here
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

    int64_t min_;
    int64_t max_;
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
