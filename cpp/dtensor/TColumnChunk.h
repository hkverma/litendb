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

  // UUID differentiates between ColumnChunks
  class TColumnChunk {

  public:

    void Print();

    int64_t sequenceNum_;
    int64_t columnNum_;
    int64_t startOffset_;
    
    arrow::Type::type type_;
    int64_t totalComponents_;
    boost::uuids::uuid uuid_;

  };
};
