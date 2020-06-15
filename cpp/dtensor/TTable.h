//
// Catalog Data
//
// Represented in C++ class for cached data from MySQL
// Store in MySQL to provide consistent & persistent catalog nodes
//
// Catalog is represented by a vector of rows. It is a flat representation of Data Tensor.
// TBD
// Stored as a MySQL table for persistence. 
// TCatalog is prepared from MySQL database. Any updated in MySQL should be updated to TCatalog.
//
#pragma once

#include <boost/uuid/uuid.hpp>
#include <arrow/api.h>
#include <vector>
#include <map>
#include "TColumnChunk.h"

namespace tendb {

  
  class TTableComponent {
    boost::uuids::uuid tColumnChunkUuid_;
    int tColumnChunkSequenceNum_;
    int tColumnNum_;
    int tStartOffset;
  };
  
  // TTable holds table values
  class TTable {
  public:

    void Init(std::shared_ptr<arrow::Table> table);
    void Print();

    std::shared_ptr<TColumnChunk> GetColumnChunk(int64_t rowNum, int64_t colNum);

    std::shared_ptr<arrow::Schema> schema_;
    int64_t numColumns_;
    int64_t numRows_;
    std::vector<std::shared_ptr<TColumn>> columns_;

  };
  
};
