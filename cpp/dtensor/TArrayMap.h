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

  };

  // zone maps for comparable types currently only range type
  class TInt64ArrayMap : public TArrayMap
  {
  public:
    TInt64ArrayMap(std::shared_ptr<arrow::Array> arr);
    int64_t min_;
    int64_t max_;
  };

  //
  // TODO Use Memory Pool to create an arrow memory pool liks  arrow::MemoryPool* pool_
  //      Use pool to store all the values
  //
  class TChunkedArrayMap {
  public:
    TChunkedArrayMap(std::shared_ptr<arrow::ChunkedArray> chunkedArray) :
      chunkedArray_(chunkedArray)
    {  }
    ~TChunkedArrayMap() { }

    static std::shared_ptr<TChunkedArrayMap> Make(std::shared_ptr<arrow::ChunkedArray> chunkedArray);

    std::shared_ptr<arrow::ChunkedArray> chunkedArray_;
    std::vector<std::shared_ptr<TArrayMap>> arrayMap_;

    arrow::Status status_;
  };

};
