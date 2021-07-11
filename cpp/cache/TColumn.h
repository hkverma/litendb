//
// Liten Columnar Storage Node
//
// Columnar Data storage
//
// One column chunk stores numComponents of a single column of Liten
// Arrow in memory, persistent in parquet format
//
#pragma once

#include <common.h>
#include <TCoreTypes.h>

//TBD See which shared pointers can be unique

namespace liten {

  class TColumn {
  public:
    
    /// Construct a column
    /// @param name of the column
    /// @param type if dimension or fact table
    /// @param chunkedArrow an arrow table that has been read 
    TColumn(std::string name,
            TableType type,
            std::shared_ptr<arrow::ChunkedArray> chunkedArray)
      : type_(type), name_(name), chunkedArray_(chunkedArray) { }

    /// Add all blocks to catalog
    Status AddToCatalog();
    
  private:    
    /// Arrow table name, must be unique
    std::string name_;
    
    /// Type of column -fact or dimension
    TableType type_;

    /// chunkedArrows from which TCache was created
    std::shared_ptr<arrow::ChunkedArray> chunkedArray_;
    
  };
  
};
