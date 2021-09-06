#pragma once

#include <common.h>
#include <cache_fwd.h>
#include <TColumnMap.h>

//TBD See which shared pointers can be unique

namespace liten
{

class TColumn
{
public:

  /// Construct a column
  TColumn(std::shared_ptr<TTable> table) : table_(table), map_(nullptr) { }

  /// Add TBlock to Column
  TStatus Add(std::shared_ptr<TBlock> tBlock);
  
  /// Add all blocks to catalog
  TStatus AddToCatalog();

  /// Get number of blocks
  int64_t NumBlocks();

  /// Total number of rows in column
  int64_t NumRows();
  
  /// Iterate over the TBlocks
  std::shared_ptr<TBlock> GetBlock(int64_t blkNum);

  template <class Type, class ArrayType>
  class Iterator
  {
  public:
    Iterator() : currentBlockRowId_(0), lastBlockRowId_(0), blockNum_(0)
    {
      array_ = std::static_pointer_cast<ArrayType>(blocks_(blockNum_)->GetArray());
      blockNum_++;
    }

    void Reset()
    {
      currentBlockRowId_=0;
      lastBlockRowId_=0;
      blockNum_=0;
      array_ = std::static_pointer_cast<ArrayType>(blocks_(blockNum_)->GetArray());
      blockNum_++;
    }

    bool Next(Type& data)
    {
      int64_t rowId = currentBlockRowId_+lastBlockRowId_;
      if (rowId >= NumRows())
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
    
  private:
    int64_t currentBlockRowId_;
    int64_t lastBlockRowId_;
    int64_t blockNum_;
    std::shared_ptr<ArrayType> array_;
    
    bool NextBlock()
    {
      blockNum_++;
      if (blockNum_ >= blocks_.size())
      {
        return false;
      }
      array_ = std::static_pointer_cast<ArrayType>(blocks_(blockNum_)->GetArray());
      return true;
    }
    
  };

  // first rowId for a given value in chunkedArray
  // TODO under progress needs to define Table such that we can skip the columns in the table
  // It leads to issues currently
  // TODO currently assumes return first match it can be a set
  //
  template<class Type, class ArrayType> 
  bool GetRowId(int64_t& arrId,            // Output array Id
                int64_t& rowId,             // output Row Id
                Type& value)                // Input Value
  {
    std::shared_ptr<arrow::ChunkedArray> chunkedArray = table->GetTable()->column(colNum);

    if (map_ || map_->IfValidMap)
    {
      bool found = colMap->GetReverseMap(value, arrId, rowId);
      return found;
    }

    // TODO Use zone map here min-max here diferentiate using what has been built
    
    // Do this if no map found
    int64_t chunkNum = 0;
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
  };

  // Get value from a rowId for a given chunkedArray
  template<class Type, class ArrayType>
  bool GetValue(int64_t& rowId,  // rowId input
                Type& value)      // output value
  {
    int64_t blkId=0;
    for (int64_t blkNum=0; blkNum<=blocks_->size(); blkNum++)
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
  bool GetValue(int64_t blkId,  // array Id
                int64_t rowId,  // rowId input                
                Type& value)      // output value
  {
    if (blkId >= blocks_.size() || blkId < 0 )
      return false;
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
  
private:
  /// chunkedArrows from which TCache was created
  std::vector<std::shared_ptr<TBlock>> blocks_;

  /// Table to which this column belongs
  std::shared_ptr<TTable> table_;

  /// map columns to create the tensor structures
  std::shared_ptr<TColumnMap> map_;

};

inline TStatus TColumn::Add(std::shared_ptr<TBlock> tBlock)
{
  if (nullptr == tBlock)
  {
    return TStatus::Invalid("Cannot add null block to column");
  }
  blocks_.push_back(tBlock);
  return TStatus::OK();
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

}
