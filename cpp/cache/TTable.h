#pragma once

#include <common.h>
#include <cache_fwd.h>
#include <TColumn.h>

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
                                                 std::shared_ptr<arrow::Table> table,
                                                 std::string schemaName);

  /// Construct a table
  /// @param name of the table
  /// @param type if dimension or fact table
  /// @param name of schema to be added
  static TResult<std::shared_ptr<TTable>> Create(std::string tableName,
                                                 TableType type,
                                                 std::string schemaName);

  /// Iterate over the TColumns
  int64_t NumColumns();

  /// Get Columns for colNum
  std::shared_ptr<TColumn> GetColumn(int64_t colNum);

  /// Get column for field
  TResult<std::shared_ptr<TColumn>> GetColumn(std::shared_ptr<arrow::Field> field);

  // GetBlock for given row and column
  std::shared_ptr<TBlock> GetBlock(int64_t rbNum, int64_t colNum);
  
  /// Get row block numbers
  int64_t NumRowBlocks();

  /// Get rowblock for rbNum
  std::shared_ptr<TRowBlock> GetRowBlock(int32_t rbNum);

  /// Append the rowblock to the table
  TResult<std::shared_ptr<TRowBlock>> AddRowBlock(std::shared_ptr<arrow::RecordBatch> rb, int64_t numRows=-1);

  /// Add Schema 
  TResult<std::shared_ptr<TSchema>> AddSchema(std::shared_ptr<arrow::Schema> schema);
  
  /// get schema of the table
  std::shared_ptr<TSchema> GetSchema();
    
  /// Print schema in logfile
  void PrintSchema();

  /// Print table in logfile
  void PrintTable();

  /// TBD Clean these up with tensor values
  /// Inverted maps and min-max zones
  TStatus CreateMaps();

  /// This will create required maps and join columns for this table
  TStatus CreateTensor();

  template<class Type, class ValueType, class ArrayType>
  TStatus CreateColumnLookUp(int64_t cnum,
                             std::shared_ptr<TColumn> col,
                             std::shared_ptr<TSchema> parentSchema,
                             std::shared_ptr<arrow::Field> parentField);
  
  void PrintMaps();

  std::string GetName();

  /// Type of table - dim or fact
  TableType GetType();
    
  int64_t NumRows();
  
  // Get different cuts. For now it is a simple index cut - minIndex <= index <= maxIndex
  // TODO do point, range, set cuts 
  std::shared_ptr<arrow::Table> Slice(int64_t offset, int64_t length);
    
  // This performs the following operation
  // 1. Find arrId, rowId where leftTable[lefttColNum][arrId,rowId] == leftValue
  // 2. Return rightValue equal to rightTable[rightColNum][arrId,rowId]
  template<class TypeLeft, class ArrayTypeLeft,
           class TypeRight=TypeLeft, class ArrayTypeRight=ArrayTypeLeft> inline
  bool JoinInner(TypeLeft& leftValue,      // leftValue Input
                 int64_t leftColNum,      // leftColNum
                 int64_t& leftRowIdInMicroseconds,   // time taken to look for leftValue
                 TypeRight& rightValue,    // rightValue output
                 int64_t rightColNum,     // right Col Num
                 int64_t& rightValueInMicroseconds)  // time taken to look for rightValue
  {
    int64_t rowId, arrId;
    TStopWatch timer;
    timer.Start();
    bool result = GetColumn(leftColNum)->GetRowId<TypeLeft, ArrayTypeLeft>(arrId, rowId, leftValue);
    timer.Stop();
    leftRowIdInMicroseconds += timer.ElapsedInMicroseconds();
    if (!result)
    {
      return result;
    }

    timer.Start();
    result = GetColumn(rightColNum)->GetValue<TypeRight, ArrayTypeRight>(arrId, rowId, rightValue);
    timer.Stop();
    rightValueInMicroseconds += timer.ElapsedInMicroseconds();
    return result;
  }
  
  template<class TypeLeft, class ArrayTypeLeft,
           class TypeRight=TypeLeft, class ArrayTypeRight=ArrayTypeLeft> inline
  bool GetValue(int64_t childColNum,                // childColNum
                int64_t childRowId,                  // childRowId
                int64_t& childRowIdInMicroseconds,   // time taken to look for leftValue
                TypeRight& parentValue,              // rightValue output
                int64_t parentColNum,                // right Col Num
                int64_t& parentValueInMicroseconds)  // time taken to look for rightValue
  {
    int64_t rowId, arrId;
    TStopWatch timer;
    bool result = true;
    timer.Start();
    if (nullptr == parentArrId_[childColNum] || nullptr == parentRowId_[childColNum])
    {
      TLOG(ERROR) << "Invalid tensor representation";
      result = false;
    }
    else
    {
      rowId = parentRowId_[childColNum]->at(childRowId);
      arrId = parentArrId_[childColNum]->at(childRowId);
    }
    timer.Stop();
    childRowIdInMicroseconds += timer.ElapsedInMicroseconds();
    if (!result)
    {
      return result;
    }

    timer.Start();
    result = parentColumn_[childColNum]->GetValue<TypeRight, ArrayTypeRight>(arrId, rowId, parentValue);
    timer.Stop();
    parentValueInMicroseconds += timer.ElapsedInMicroseconds();
    return result;
  }

  // TBD Add to the option class
  static const bool EnableColumnReverseMap = false;

  /// Destructor
  ~TTable() { }
    
private:

  /// Use named constructor only
  TTable() : type_(FactTable), numRows_(0), schema_(nullptr) { }

  /// Enable shared constructor
  struct MakeSharedEnabler;
    
  /// table name, must be unique
  std::string name_;
    
  /// Schema name associated with this table
  std::string schemaName_;
  
  /// Type of table -fact or dimension
  TableType type_;
    
  /// Tables consist of columnar series
  std::vector<std::shared_ptr<TColumn>> columns_;
  std::unordered_map<std::shared_ptr<arrow::Field>, std::shared_ptr<TColumn>> fieldToColumns_;

  /// Joined parent tables of the tensor TBD Use Arrow array builder
  std::vector<std::shared_ptr<std::vector<int64_t>>> parentArrId_;
  std::vector<std::shared_ptr<std::vector<int64_t>>> parentRowId_;
  std::vector<std::shared_ptr<TColumn>> parentColumn_;
  
  /// Tables consist of columnar series
  std::vector<std::shared_ptr<TRowBlock>> rowBlocks_;

  /// Total number of rows so far
  int64_t numRows_;
  
  /// Schema of the table
  std::shared_ptr<TSchema> schema_;
  
  // Add all columns to catalog TBD Catalog cleanups
  TStatus AddToCatalog();
      
};

// Enable shared_ptr with private constructors
struct TTable::MakeSharedEnabler : public TTable {
  MakeSharedEnabler() : TTable() { }
};


inline std::shared_ptr<TSchema> TTable::GetSchema()
{
  return schema_;
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
  return columns_.size();
}
  
inline std::shared_ptr<TColumn> TTable::GetColumn(int64_t colNum)
{
  if (colNum < 0 || colNum > columns_.size())
    return nullptr;
  return columns_[colNum];
}

inline int64_t TTable::NumRowBlocks()
{
  return rowBlocks_.size();
}

inline std::shared_ptr<TRowBlock> TTable::GetRowBlock(int32_t rbNum)
{
  if (rbNum < 0 || rbNum > rowBlocks_.size())
    return nullptr;
  return rowBlocks_[rbNum];
}

inline int64_t TTable::NumRows()
{
  return numRows_;
}
  
}
