#pragma once

#include <common.h>
#include <cache_fwd.h>
#include <TSchema.h>
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
  TResult<std::shared_ptr<TRowBlock>> AddRowBlock(std::shared_ptr<arrow::RecordBatch> rb);

  /// Append arrow table to the table
  TStatus AddArrowTable(std::shared_ptr<arrow::Table> table);

  /// Add Schema
  TResult<std::shared_ptr<TSchema>> AddSchema(std::shared_ptr<arrow::Schema> schema);

  /// get schema of the table
  std::shared_ptr<TSchema> GetSchema();

  /// Print schema in logfile
  void PrintSchema();

  /// Print table in logfile
  void PrintTable(bool columns, bool parents);
  std::string ToString();
  std::string ParentsToString();

  /// TBD Clean these up with tensor values
  /// Inverted maps and min-max zones
  TStatus MakeMaps(bool ifReverseMap=false);

  /// This will create required maps and join columns for this table
  TStatus MakeTensor();

  template<class Type, class ValueType, class ArrayType>
  TStatus CreateColumnLookUp(int64_t cnum,
                             std::shared_ptr<TColumn> col,
                             std::shared_ptr<TSchema> parentSchema,
                             std::shared_ptr<arrow::Field> parentField);

  void PrintMaps();

  std::string GetName();

  /// Type of table - dim or fact
  TableType GetType();

  int64_t NumRows() { return numRows_; }

  // Get different cuts. For now it is a simple index cut - minIndex <= index <= maxIndex
  // TODO do point, range, set cuts
  std::shared_ptr<arrow::Table> Slice(int64_t offset, int64_t length);

  // This performs the following operation
  // 1. Find arrId, rowId where leftTable[lefttColNum][arrId,rowId] == leftValue
  // 2. Return rightValue equal to rightTable[rightColNum][arrId,rowId]
  // TBD Clean it up
  template<class TypeLeft, class ArrayTypeLeft,
           class TypeRight=TypeLeft, class ArrayTypeRight=ArrayTypeLeft> inline
  bool JoinInner(TypeLeft& leftValue,      // leftValue Input
                 int64_t leftColNum,      // leftColNum
                 int64_t& leftRowIdInMicroseconds,   // time taken to look for leftValue
                 TypeRight& rightValue,    // rightValue output
                 int64_t rightColNum,     // right Col Num
                 int64_t& rightValueInMicroseconds)  // time taken to look for rightValue
  {
    TStopWatch timer;
    timer.Start();
    TRowId rowId = GetColumn(leftColNum)->GetRowId<TypeLeft, ArrayTypeLeft>(leftValue);
    timer.Stop();
    leftRowIdInMicroseconds += timer.ElapsedInMicroseconds();
    if (rowId.blkNum < 0)
    {
      return false;
    }

    timer.Start();
    bool result = false;
    auto resultValue = GetColumn(rightColNum)->GetValue<TypeRight, ArrayTypeRight>(rowId);
    if (resultValue.ok())
    {
      rightValue = resultValue.ValueOrDie();
      result = true;
    }
    timer.Stop();
    rightValueInMicroseconds += timer.ElapsedInMicroseconds();
    return result;
  }


  /// Get parent column info
  TRowId GetParentInfo(int64_t colNum, TRowId id);

  /// Current table to child. GetValue looks at the parent table joined by the column parentColId
  template<class Type, class ArrayType> inline
  TResult<Type> GetValue(TRowId rowId, // rowId
                         int64_t colId, // colId
                         int64_t parentColId, // parent Col Id
                         TRowId& parentRowId, // Return parent rowId for hieararchical lookups
                         std::vector<int64_t>& wallTimeInMicroSeconds); // time taken


  /// Current table to child. GetValue looks at the parent table joined by two hierarchies
  template<class Type, class ArrayType> inline
  TResult<Type> GetValue(TRowId rowId,       // rowId
                         int64_t colId,     // col Id
                         int64_t parent0ColId, // parent0 Col Id to be joined
                         int64_t parent1ColId, // parent1 Col Id to be joined
                         std::vector<int64_t>& wallTimeInMicroSeconds); // time taken


  /// Current table to child. GetValue looks at the parent table joined by the column number
  /// It gets the parent column number value
  /// TBD ArrayType can be derived from Type
  /*REMOVE  template<class Type, class ArrayType> inline
  bool GetValue(int64_t colNum,                // colNum
                int64_t rowId,                  // rowId
                int64_t& rowIdInMicroseconds,   // time taken to look for leftValue
                Type& parentValue,              // parent Value
                int64_t parentColNum,                // parent Col Num
                int64_t& parentValueInMicroseconds)  // time taken to look for parent Value
  {
    int64_t parentRowId, parentArrId;
    TStopWatch timer;
    bool result = true;
    timer.Start();
    if (nullptr == parentBlkNum_[colNum] ||
        nullptr == parentRowNum_[colNum] ||
        nullptr == parentColumn_[colNum])
    {
      TLOG(ERROR) << "Invalid tensor representation";
      result = false;
    }
    else
    {
      parentRowId = parentRowNum_[colNum]->at(rowId);
      parentArrId = parentBlkNum_[colNum]->at(rowId);
    }
    timer.Stop();
    rowIdInMicroseconds += timer.ElapsedInMicroseconds();
    if (!result)
    {
      return result;
    }

    timer.Start();
    auto parentTable = parentColumn_[colNum]->GetTable();
    auto parentColumn = parentTable->GetColumn(parentColNum);
    result = parentColumn->GetValue<Type, ArrayType>(parentArrId, parentRowId, parentValue);
    timer.Stop();
    parentValueInMicroseconds += timer.ElapsedInMicroseconds();
    if (!result)
    {
      TLOG(ERROR) << "Invalid parent lookup";
    }
    return result;
    }*/

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

  /// Parent columns for each column in the table, null if no parents
  /// For each value, the parent rowNum and blkNum are also stored if parentCol is not null
  std::vector<std::shared_ptr<std::vector<std::vector<TRowId>>>> parentRowIdLookup_;
  std::vector<std::shared_ptr<TColumn>> parentColumn_;

  /// Tables consist of columnar series
  std::vector<std::shared_ptr<TRowBlock>> rowBlocks_;

  /// Total number of rows in the table
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

/// Current table to child. GetValue looks at the parent table joined by the column parentColId
template<class Type, class ArrayType> inline
TResult<Type> TTable::GetValue(TRowId rowId, // rowId
                               int64_t colId, // colId
                               int64_t parentColId, // parent Col Id
                               TRowId& parentRowId, // Return parent rowId for hieararchical joins
                               std::vector<int64_t>& wallTimeInMicroSeconds) // time taken
{
  TStopWatch timer;
  timer.Start();
  parentRowId = GetParentInfo(colId, rowId);
  timer.Stop();
  wallTimeInMicroSeconds[0] += timer.ElapsedInMicroseconds();
  if (parentRowId.blkNum < 0)
  {
    return TStatus::Invalid("Invalid tensor representation");
  }

  timer.Start();
  auto parentColumn = parentColumn_[colId]->GetTable()->GetColumn(parentColId);
  auto value = std::move(parentColumn->GetValue<Type, ArrayType>(parentRowId));
  timer.Stop();
  wallTimeInMicroSeconds[1] += timer.ElapsedInMicroseconds();
  return value;
}

template<class Type, class ArrayType> inline
TResult<Type> TTable::GetValue(TRowId rowId,       // rowId
                               int64_t colId,     // col Id
                               int64_t parent0ColId, // parent0 Col Id to be joined
                               int64_t parent1ColId, // parent1 Col Id to be joined
                               std::vector<int64_t>& wallTimeInMicroSeconds) // time taken
{

  TStopWatch timer;

  timer.Start();
  auto parent0RowId = GetParentInfo(colId, rowId);
  if (parent0RowId.blkNum < 0)
  {
    return TStatus::Invalid("Invalid tensor representation");
  }
  timer.Stop();
  wallTimeInMicroSeconds[0] += timer.ElapsedInMicroseconds();

  timer.Start();
  auto parent0Table = parentColumn_[colId]->GetTable();
  auto parent1RowId = parent0Table->GetParentInfo(parent0ColId, parent0RowId);
  if (parent1RowId.blkNum < 0)
  {
    return TStatus::Invalid("Invalid tensor representation");
  }
  timer.Stop();
  wallTimeInMicroSeconds[1] += timer.ElapsedInMicroseconds();

  timer.Start();
  auto parent1Column = parent0Table->parentColumn_[parent0ColId]->GetTable()->GetColumn(parent1ColId);
  auto value = std::move(parent1Column->GetValue<Type, ArrayType>(parent1RowId));
  timer.Stop();
  wallTimeInMicroSeconds[2] += timer.ElapsedInMicroseconds();

  return value;

}

inline TRowId TTable::GetParentInfo(int64_t colNum, TRowId id)
{
  TRowId pId;
  auto lkup = parentRowIdLookup_[colNum];
  if (nullptr == lkup)
    return pId;

  pId = (*lkup)[id.blkNum][id.rowNum];
  return pId;
}

template<class Type, class ValueType, class ArrayType>
TStatus TTable::CreateColumnLookUp(int64_t cnum,
                                   std::shared_ptr<TColumn> col,
                                   std::shared_ptr<TSchema> parentSchema,
                                   std::shared_ptr<arrow::Field> parentField)
{

  if (nullptr != parentRowIdLookup_[cnum])
  {
    TLOG(INFO) << "Trying to rereate Join. child table=" << GetName() << " col=" << col->GetName();
    return TStatus::OK();
  }

  // Get parent column
  auto ttable = parentSchema->GetTable();
  if (!ttable)
  {
    return TStatus::Invalid("No table for parent schema.");
  }

  auto parentColResult = ttable->GetColumn(parentField);
  if (!parentColResult.ok())
    return std::move(parentColResult.status());
  auto parentCol = parentColResult.ValueOrDie();
  if (parentCol->GetMap().ok())
  {
    parentCol->CreateReverseMap();
  }

  // For each block, create a vector to look at the parent value
  auto parentRowIdLookup =
    std::make_shared<std::vector<std::vector<TRowId>>>(col->NumBlocks());

  for (auto blkNum=0; blkNum<col->NumBlocks(); blkNum++)
  {
    std::shared_ptr<arrow::NumericArray<ValueType>> arr =
      std::dynamic_pointer_cast<arrow::NumericArray<ValueType>>(col->GetBlock(blkNum)->GetArray());
    if (nullptr == arr)
    {
      return TStatus::Invalid("Non numeric array not supported in Column lookups");
    }

    std::vector<TRowId> parentRowId(arr->length());

    for (auto rowNum=0; rowNum<arr->length(); rowNum++)
    {
      Type value = std::move(arr->Value(rowNum));
      parentRowId[rowNum] = std::move(parentCol->GetRowId<Type,ArrayType>(value));
    }
    parentRowIdLookup->at(blkNum) = std::move(parentRowId);
  }

  // Now set the parent Column
  parentColumn_[cnum] = parentCol;
  parentRowIdLookup_[cnum] = parentRowIdLookup;

  TLOG(INFO) << "Joined child table=" << GetName() << " col=" << col->GetName()
             << " parent table=" << ttable->GetName() << " col=" << parentCol->GetName();

  return TStatus::OK();
}

}
