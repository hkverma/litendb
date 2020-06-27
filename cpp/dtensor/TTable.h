//
// Catalog Data
//
// Represented in C++ class for cached data from Redis
// Store in Redis to provide in-memory fast, as well as consistent & persistent catalog nodes
//
// Catalog is represented by a vector of rows. It is a flat representation of Data Tensor.
// TBD
// Stored as a Redis table for persistence. 
// TCatalog is prepared from MySQL database. Any updated in MySQL should be updated to TCatalog.
//
#pragma once

#include <boost/uuid/uuid.hpp>
#include <arrow/api.h>
#include <vector>
#include <map>
#include "TColumnChunk.h"

namespace tendb {
  
  // TTable holds table values
  class TTable {
  public:

    TTable() : table_(nullptr), schema_(nullptr) { }  
    TTable(std::shared_ptr<arrow::Table> table);
    
    void Print();
    bool Read(std::string csvFileName);
    
    std::shared_ptr<arrow::Array> GetArray(int64_t rowNum, int64_t colNum);

    int64_t NumColumns() { return table_->num_columns(); }
    int64_t NumRows() { return table_->num_rows(); }
    
    std::shared_ptr<arrow::Schema> schema_;
    std::shared_ptr<arrow::Table> table_;    
    
  };
  
};
