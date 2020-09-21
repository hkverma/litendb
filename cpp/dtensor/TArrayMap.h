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

  // Abstract Base Array Map
  class TArrayMap {
  public:
    TArrayMap(std::shared_ptr<arrow::ChunkedArray> chunkedArray) : chunkedArray_(chunkedArray) { }
    ~TArrayMap() { }

    static std::shared_ptr<TArrayMap> Make(std::shared_ptr<arrow::ChunkedArray> chunkedArray);

    std::shared_ptr<arrow::ChunkedArray> chunkedArray_;
    arrow::MemoryPool* pool_;
    arrow::Status status_;
    
  };

  // zone maps for comparable types currently only range type  
  class TInt64ArrayMap : public TArrayMap
  {
  public:
    TInt64ArrayMap(std::shared_ptr<arrow::ChunkedArray> chunkedArray) : TArrayMap(chunkedArray) { }
    int64_t min_;
    int64_t max_;
  };

};
