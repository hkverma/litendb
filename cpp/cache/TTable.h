#pragma once

#include <common.h>
#include <cache_fwd.h>

namespace liten
{
  
// TTable holds table values
class TTable : public std::enable_shared_from_this<TTable>
{
public:
    
  /// Construct a table
  /// @param name of the table
  /// @param type if dimension or fact table
  /// @param table an arrow table that has been read 
  static TResult<std::shared_ptr<TTable>> Create(std::string tableName,
                                                 TableType type,
                                                 std::shared_ptr<arrow::Table> table);

  /// Get underlying arrow::Table
  std::shared_ptr<arrow::Table> GetTable();
    
  /// Print schema in logfile
  void PrintSchema();

  /// Print table in logfile
  void PrintTable();

  /// TBD Clean these up with tensor values
  /// Inverted maps and min-max zones
  int MakeMaps(int32_t numCopies);
  void PrintMaps();
  std::shared_ptr<TColumnMap> GetColMap(int mapNum, int colNum);
  std::shared_ptr<arrow::Array> GetArray(int64_t rowNum, int64_t colNum);
  std::string GetName();

  /// Type of table - dim or fact
  TableType GetType();
    
  int64_t NumColumns();
  int64_t NumRows();

  // Get different cuts. For now it is a simple index cut - minIndex <= index <= maxIndex
  // TODO do point, range, set cuts 
  std::shared_ptr<arrow::Table> Slice(int64_t offset, int64_t length);
    
  // TBD Add to the option class
  static const bool EnableColumnReverseMap = false;

  /// Destructor
  ~TTable() { }
    
private:

  /// Use named constructor only
  TTable() { }

  /// Enable shared constructor
  struct MakeSharedEnabler;
    
  /// Arrow table name, must be unique
  std::string name_;
    
  /// Type of table -fact or dimension
  TableType type_;
    
  /// Tables consist of columnar series
  std::vector<std::shared_ptr<TColumn>> columns_;
    
  /// Tables consist of columnar series
  std::vector<std::shared_ptr<TRowBlock>> rowBlocks_;
    
  /// Schema of the table
  std::shared_ptr<TSchema> schema_;
    
  /// Arrow table from which this table was created
  std::shared_ptr<arrow::Table> table_;
    
  // Add all columns to catalog
  TStatus AddToCatalog();
      
  // Table Maps
  // TODO One copy should be sufficient, multiple copies will not make it faster
  int32_t numMapCopies_ = 0;
  std::vector<std::vector<std::shared_ptr<TColumnMap>>> maps_;
    
};

// Enable shared_ptr with private constructors
struct TTable::MakeSharedEnabler : public TTable {
  MakeSharedEnabler() : TTable() { }
};

// TBD Clean all these functions
  
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

inline TableType TTable::GetType()
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

}
