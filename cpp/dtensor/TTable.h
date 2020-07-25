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
#include <vector>
#include <map>
#include <boost/uuid/uuid.hpp>
// Need scalar before arrow/api.h to access full class definitions
#include <arrow/scalar.h>
#include <arrow/api.h>
#include <arrow/csv/api.h>

#pragma once

namespace tendb {
  
  // TTable holds table values
  class TTable {
  public:

    TTable(std::string& tableName) : name_(tableName), table_(nullptr), schema_(nullptr) { }  
    TTable(std::shared_ptr<arrow::Table> table);
    
    void Print();
    /// read csv File csvFileName and add it to table_
    bool ReadCsv(std::string csvFileName,
                 const arrow::csv::ReadOptions& readOptions,
                 const arrow::csv::ParseOptions& parseOptions,
                 const arrow::csv::ConvertOptions& convertOptions);
    
    std::shared_ptr<arrow::Array> GetArray(int64_t rowNum, int64_t colNum);

    int64_t NumColumns() { return table_->num_columns(); }
    int64_t NumRows() { return table_->num_rows(); }

    std::shared_ptr<arrow::Table> table_;
  private:
    std::shared_ptr<arrow::Schema> schema_;
    std::string name_;
    
  };
  
};
