#pragma once

#include <common.h>
#include <cache_fwd.h>
#include <TBlock.h>
#include <TColumnMap.h>

//TBD See which shared pointers can be unique

namespace liten
{

/// Each block in Liten Column has a unique blkNum. rowNum is the rownumber in that block.
struct TRowId {
  TRowId() : blkNum(-1), rowNum(-1) { }
  int32_t blkNum;
  int32_t rowNum;
};

class TColumn : public std::enable_shared_from_this<TColumn>
{
public:

  /// Construct a column
  TColumn(std::shared_ptr<TTable> table, std::shared_ptr<arrow::Field> field) :
    table_(table), map_(nullptr), numRows_(0), field_(field) {  name_ = field->name(); }

  /// Add TBlock to Column
  TStatus Add(std::shared_ptr<TBlock> tBlock);
  
  /// Get number of blocks
  int64_t NumBlocks();

  /// Total number of rows in column
  int64_t NumRows();
  
  /// Get blkNum block, null if out of range
  std::shared_ptr<TBlock> GetBlock(int64_t blkNum);

  /// Get associated table
  std::shared_ptr<TTable> GetTable();

  std::shared_ptr<arrow::ChunkedArray> Slice(int64_t offset, int64_t length);

  /// Get col name
  std::string GetName() { return name_; }
  
  /// Get the map if already exists, else create one
  TResult<std::shared_ptr<TColumnMap>>  GetMap();
  TStatus CreateZoneMap(bool forceCreate=false);
  TStatus CreateReverseMap(bool forceCreate=false);
  
  /// Get map for this column
  std::shared_ptr<TColumnMap> GetCurMap() { return map_; }

  /// String debug output
  std::string ToString(bool values, bool zonemap, bool reversemap);
  
  /// A simple forward iterator for TColumn
  template <class Type, class ArrayType>
  class Iterator
  {
  public:
    
    Iterator(std::shared_ptr<TColumn> tcolumn);
    void Reset();
    bool Next(Type& data);
    
  private:
    int64_t currentBlockRowId_;
    int64_t lastBlockRowId_;
    int64_t blockNum_;
    std::shared_ptr<ArrayType> array_;
    std::shared_ptr<TColumn> tcolumn_;
    
    bool NextBlock();
    
  };

  /// first rowId for a given value 
  template<class Type, class ArrayType> 
  TRowId GetRowId(Type& value);

  template<class Type, class ArrayType> 
  TRowId GetRowIdLinear(Type& value);

  /// Given rowNum get blkId and rowId
  TRowId GetRowId(int64_t rowNum);
  
  /// Get value from rowid
  template<class Type, class ArrayType> inline
  TResult<Type> GetValue(TRowId& id);

  /// This is currently slow, get value from a linear value of rowId
  template<class Type, class ArrayType>
  TResult<Type> GetValue(int64_t rowId);  // rowId input

  int64_t GetRowNum(TRowId rowId);
  
  arrow::Result<std::shared_ptr<arrow::Scalar>> GetScalar(int64_t rowId);
  arrow::Result<std::shared_ptr<arrow::Scalar>> GetScalar(int64_t arrId, int64_t rowId);
  

private:
  
  /// chunkedArrows from which TCache was created
  std::vector<std::shared_ptr<TBlock>> blocks_;

  /// total rows in columns
  int64_t numRows_;

  /// field associated with this column
  std::shared_ptr<arrow::Field> field_;

  /// name of the column same as field
  std::string name_;

  /// Table to which this column belongs
  std::shared_ptr<TTable> table_;

  /// map columns to create the tensor structures
  std::shared_ptr<TColumnMap> map_;

};

inline int64_t TColumn::NumRows()
{
  return numRows_;
}

/// Get number of blocks
inline int64_t TColumn::NumBlocks()
{
  return blocks_.size();
}

/// Get number of blocks
inline std::shared_ptr<TBlock> TColumn::GetBlock(int64_t blkNum)
{
  if (blkNum < 0 || blkNum >= blocks_.size())
    return nullptr;
  return blocks_[blkNum];
}

inline std::shared_ptr<TTable> TColumn::GetTable()
{
  return table_;
}

template <class Type, class ArrayType>
TColumn::Iterator<Type, ArrayType>::Iterator(std::shared_ptr<TColumn> tcolumn) :
  tcolumn_(tcolumn), currentBlockRowId_(0), lastBlockRowId_(0), blockNum_(0)
{
  array_ = std::static_pointer_cast<ArrayType>(tcolumn_->blocks_[blockNum_]->GetArray());
  blockNum_++;
}

template <class Type, class ArrayType>
void TColumn::Iterator<Type, ArrayType>::Reset()
{
  currentBlockRowId_=0;
  lastBlockRowId_=0;
  blockNum_=0;
  array_ = std::static_pointer_cast<ArrayType>(tcolumn_->blocks_[blockNum_]->GetArray());
  blockNum_++;
}

template <class Type, class ArrayType>
bool TColumn::Iterator<Type, ArrayType>::Next(Type& data)
{
  int64_t rowId = currentBlockRowId_+lastBlockRowId_;
  if (rowId >= tcolumn_->NumRows())
    return false;
    
  if (currentBlockRowId_ >= array_->length())
  {
    if (!NextBlock())
    {
      return false;
    }
    lastBlockRowId_ += currentBlockRowId_;
    currentBlockRowId_ = 0;
  }
  data = array_->Value(currentBlockRowId_);
  currentBlockRowId_++;
  return true;
}

template <class Type, class ArrayType>
bool TColumn::Iterator<Type, ArrayType>::NextBlock()
{
  blockNum_++;
  if (blockNum_ >= tcolumn_->blocks_.size())
  {
    return false;
  }
  array_ = std::static_pointer_cast<ArrayType>(tcolumn_->blocks_[blockNum_]->GetArray());
  return true;
}

template<class Type, class ArrayType> inline
TRowId TColumn::GetRowId(Type& value) // Input Value
{
  TRowId id;
  if (map_ && map_->IfValidReverseMap())
  {
    id = map_->GetReverseMap(value);
  }
  return id;
}

template<class Type, class ArrayType> inline
TRowId TColumn::GetRowIdLinear(Type& value) // Input Value
{
  TRowId id;
  for (auto arrId=0; arrId <blocks_.size(); arrId++)
  {
    std::shared_ptr<ArrayType> arr = std::static_pointer_cast<ArrayType>(blocks_[arrId]->GetArray());
    for (auto rowId=0; rowId<arr->length(); rowId++)
    {
      if (value == arr->Value(rowId))
      {
        id.blkNum = arrId;
        id.rowNum = rowId;
        return id;
      }
    }
  }
  return id;
}


template<class Type, class ArrayType> inline
TResult<Type> TColumn::GetValue(int64_t rowId)  // rowId input
{
  auto id = GetRowId(rowId);
  auto val = std::move(GetValue<Type, ArrayType>(id));
  return val;
}  
  
// Get value from a rowId 
template<class Type, class ArrayType> inline
TResult<Type> TColumn::GetValue(TRowId& id)
{
  if (id.blkNum >= blocks_.size() || id.blkNum < 0 )
  {
    return TStatus::Invalid("Invalid block id");
  }
  std::shared_ptr<ArrayType> array =
    std::static_pointer_cast<ArrayType>(blocks_[id.blkNum]->GetArray());
  if (id.rowNum >= array->length())
  {
    return TStatus::Invalid("Invalid row id");
  }
  Type value;
  if constexpr(std::is_same_v<Type, arrow::util::string_view>)
  {
    value = array->GetView(id.rowNum);
  }
  else if constexpr(std::is_same_v<Type, std::string>)
  {
    value = std::move(array->GetString(id.rowNum));
  }
  else
  {
    value = std::move(array->Value(id.rowNum));
  }
  return value;
}

}
