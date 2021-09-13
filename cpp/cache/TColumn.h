#pragma once

#include <common.h>
#include <cache_fwd.h>
#include <TBlock.h>
#include <TColumnMap.h>

//TBD See which shared pointers can be unique

namespace liten
{

class TColumn : public std::enable_shared_from_this<TColumn>
{
public:

  /// Construct a column
  TColumn(std::shared_ptr<TTable> table, std::shared_ptr<arrow::Field> field) : table_(table), map_(nullptr), numRows_(0), field_(field) { }

  /// Add TBlock to Column
  TStatus Add(std::shared_ptr<TBlock> tBlock);
  
  /// Get number of blocks
  int64_t NumBlocks();

  /// Total number of rows in column
  int64_t NumRows();
  
  /// Get blkNum block, null if out of range
  std::shared_ptr<TBlock> GetBlock(int64_t blkNum);

  /// Create a column map - this includes min-max and reverse index
  TStatus CreateMap(bool zoneMap, bool reverseMap);

  /// Get map for this column
  std::shared_ptr<TColumnMap> GetMap() { return map_; }

  /// Get map for this column, only done by TColumnMap::Create
  void SetMap(std::shared_ptr<TColumnMap> map) { map_=map; }
  
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

  // first rowId for a given value in chunkedArray
  // TBD under progress needs to define Table such that we can skip the columns in the table
  // It leads to issues currently
  // TBD currently assumes return first match it can be a set
  // TBD Use zone map here min-max here diferentiate using what has been built
  //
  template<class Type, class ArrayType> 
  bool GetRowId(int64_t& arrId,            // Output array Id
                int64_t& rowId,             // output Row Id
                Type& value);                // Input Value

  // Get value from a rowId 
  template<class Type, class ArrayType>
  bool GetValue(int64_t& rowId,  // rowId input
                Type& value);      // output value
  
  // Get value from a rowId 
  template<class Type, class ArrayType>
  bool GetValue(int64_t blkId,  // array Id
                int64_t rowId,  // rowId input                
                Type& value);    // output value
  
private:
  /// chunkedArrows from which TCache was created
  std::vector<std::shared_ptr<TBlock>> blocks_;

  /// total rows in columns
  int64_t numRows_;

  /// field associated with this column
  std::shared_ptr<arrow::Field> field_;

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

template<class Type, class ArrayType> 
bool TColumn::GetRowId(int64_t& arrId,            // Output array Id
                       int64_t& rowId,             // output Row Id
                       Type& value)                // Input Value
{
  if (map_ && map_->IfValidReverseMap())
  {
    bool found = map_->GetReverseMap(value, arrId, rowId);
    if (!found)
      TLOG(INFO) << "Not found value " << value << " for field=" << field_->name();
    return found;
  }

  TLOG(INFO) << "No reverse map for field=" << field_->name();
  
  // Linear search if no map found
  for (arrId=0; arrId <blocks_.size(); arrId++)
  {
    std::shared_ptr<ArrayType> arr = std::static_pointer_cast<ArrayType>(blocks_[arrId]->GetArray());
    for (rowId=0; rowId<arr->length(); rowId++)
    {
      if (value == arr->Value(rowId))
      {
        return true;
      }
    }
  }
  return false;
}

template<class Type, class ArrayType>
bool TColumn::GetValue(int64_t& rowId,  // rowId input
                       Type& value)      // output value
{
  int64_t blkId=0;
  for (int64_t blkNum=0; blkNum<=blocks_.size(); blkNum++)
  {
    std::shared_ptr<ArrayType> array =
      std::static_pointer_cast<ArrayType>(blocks_[blkNum]);
    int64_t length = array->length();
    if (blkId+length < rowId)
    {
      blkId += length;
    }
    else
    {
      int64_t offset = rowId-blkId;
      if constexpr(std::is_same_v<Type, arrow::util::string_view>)
        {
          value = array->GetView(offset);
        }
      else if constexpr(std::is_same_v<Type, std::string>)
        {
          value = array->GetString(offset);
        }
      else
      {
        value = array->Value(offset);
      }
      return true;
    }
  }
  return false;
}
  
// Get value from a rowId 
template<class Type, class ArrayType>
bool TColumn::GetValue(int64_t blkId,  // array Id
                       int64_t rowId,  // rowId input                
                       Type& value)      // output value
{
  if (blkId >= blocks_.size() || blkId < 0 )
  {
    return false;
  }
  std::shared_ptr<ArrayType> array =
    std::static_pointer_cast<ArrayType>(blocks_[blkId]->GetArray());
  if (rowId >= array->length())
    return false;
    
  if constexpr(std::is_same_v<Type, arrow::util::string_view>)
    {
      value = array->GetView(rowId);
    }
  else if constexpr(std::is_same_v<Type, std::string>)
    {
      value = array->GetString(rowId);
    }
  else
  {
    value = array->Value(rowId);
  }
  return true;
}

}
