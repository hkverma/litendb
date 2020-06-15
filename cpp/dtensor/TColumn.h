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

  class TColumn {
  public:

    std::shared_ptr<TColumnChunk> GetColumnChunk(int64_t rowNum);
    uint32_t columnNum_;
    uint32_t startOffset;

    arrow::Type::type type_;
    uint32_t totalChunks_;
    uint32_t totalComponents_;

    std::vector<int64_t> startOffsets_;
    std::vector<std::shared_ptr<TColumnChunk>> columnChunks_;

  };
};
