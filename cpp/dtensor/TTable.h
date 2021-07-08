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

//#include <vector>
//#include <map>
//#include <boost/uuid/uuid.hpp>
// Need scalar before arrow/api.h to access full class definitions
//#include <arrow/scalar.h>
//#include <arrow/api.h>
//#include <arrow/csv/api.h>

#include <common.h>

#include <TColumnMap.h>

class TColumn;
class TRowBlock;

namespace liten {
  
  // TTable holds table values
  class TTable {
  public:

    /// Tables are either dimension or fact tables
    enum Type {Dimension=0, Fact};

    /// Construct a table
    /// @param name of the table
    /// @param type if dimension or fact table
    /// @param uri is a uniform resource allocator for raw file
    TTable(std::string name, Type type, std::string uri);

    /// Construct a table
    /// @param name of the table
    /// @param type if dimension or fact table
    /// @param table an arrow table that has been read 
    TTable(std::string name, Type type, std::shared_ptr<arrow::Table> table);

    // $$$$$$$
    void PrintSchema();
    void PrintTable();

    // Inverted maps and min-max zones
    int MakeMaps(int32_t numCopies);
    void PrintMaps();
    std::shared_ptr<TColumnMap> GetColMap(int mapNum, int colNum);
    std::shared_ptr<arrow::Table> GetTable();
    std::shared_ptr<arrow::Array> GetArray(int64_t rowNum, int64_t colNum);
    std::string GetName();

    /// Type of table - dim or fact
    Type GetType();
    
    int64_t NumColumns();
    int64_t NumRows();

    // Get different cuts. For now it is a simple index cut - minIndex <= index <= maxIndex
    // TODO do point, range, set cuts 
    std::shared_ptr<arrow::Table> Slice(int64_t offset, int64_t length);
    
    // Options TODO create an options class
    static const bool EnableColumnReverseMap = false;
    
  private:
    /// Arrow table name, must be unique
    std::string name_;
    /// Type of table -fact or dimension
    Type type_;
    /// Tables consist of columnar series
    std::vector<std::shared_ptr<TColumn>> columns_;
    /// Tables consist of columnar series
    std::vector<std::shared_ptr<TRowBlock>> rowBlocks_;
    /// Schema of the table
    std::shared_ptr<arrow::Schema> schema_;
    
    // TBD std::shared_ptr<arrow::Table> table_;
    // Table Maps
    // TODO One copy should be sufficient, multiple copies will not make it faster
    int32_t numMapCopies_ = 0;
    std::vector<std::vector<std::shared_ptr<TColumnMap>>> maps_;
    
  };

  // Header functions
  inline std::shared_ptr<arrow::Table> TTable::GetTable()
  {
    return table_;
  }

  inline std::shared_ptr<TColumnMap> TTable::GetColMap(int mapNum, int colNum)
  {
    return maps_[mapNum][colNum];
  }

  inline std::string TTable::GetName()
  {
    return name_;
  }

  inline TTable::TType TTable::GetType()
  {
    return type_;
  }

  inline int64_t TTable::NumColumns()
  {
    return table_->num_columns();
  }
  
  inline int64_t TTable::NumRows()
  {
    return table_->num_rows();
  }

};
