//
// TenDB Columnar Storage Node
//
// Columnar Data storage
//
// One column chunk stores numComponents of a single column of TenDB
// Arrow in memory, persistent in parquet format
//
// TODO Use Status to return value wrapped with a message
//
#pragma once

#include <boost/uuid/uuid.hpp>
#include <arrow/api.h>
#include <vector>
#include <iostream>

#include <TTable.h>
#include <common.h>

namespace tendb {

  // arrow::ChunkedArray Iterator
  template <class Type, class ArrayType>
  class TColumnIterator
  {
  public:
    TColumnIterator(std::shared_ptr<arrow::ChunkedArray> chary) :
      currentArrayRowId_(0), lastArrayRowId_(0), chunkNum_(0)
    {
      chunkedArray_ = chary;
      array_ =
        std::static_pointer_cast<ArrayType>(chunkedArray_->chunk(chunkNum_));
      chunkNum_++;
    }

    void reset()
    {
      currentArrayRowId_=0;
      lastArrayRowId_=0;
      chunkNum_=0;
      array_ =
        std::static_pointer_cast<ArrayType>(chunkedArray_->chunk(chunkNum_));
      chunkNum_++;
    }

    bool next(Type& data)
    {
      int64_t rowId = currentArrayRowId_+lastArrayRowId_;
      if (rowId >= chunkedArray_->length())
        return false;

      while (currentArrayRowId_ >= array_->length())
      {
        if (!nextArray())
        {
          return false;
        }
        lastArrayRowId_ += currentArrayRowId_;
        currentArrayRowId_ = 0;
      }
      data = array_->Value(currentArrayRowId_);
      currentArrayRowId_++;
      return true;
    }

    bool nextArray()
    {
      chunkNum_++;
      if (chunkNum_ >= chunkedArray_->num_chunks())
      {
        return false;
      }
      array_ =
        std::static_pointer_cast<ArrayType>(chunkedArray_->chunk(chunkNum_));
      return true;
    }

    int64_t currentArrayRowId_;
    int64_t lastArrayRowId_;
    int64_t chunkNum_;
    std::shared_ptr<ArrayType> array_;
    std::shared_ptr<arrow::ChunkedArray> chunkedArray_;
  };

  // first rowId for a given value in chunkedArray
  // TODO under progress needs to define Table such that we can skip the columns in the table
  // It leads to issues currently
  // TODO currently assumes return first match it can be a set
  //
  template<class Type, class ArrayType>
  bool GetRowId(int64_t& rowId,               // Row Id
                Type& value,                   // Output Value
                std::shared_ptr<TTable> table, // TTable
                int64_t colNum)                // Column Number
  {
    std::shared_ptr<arrow::ChunkedArray> chunkedArray = table->table_->column(colNum);

    bool mapExists = false;
    Type minVal, maxVal;
    if (std::is_same<Type, int64_t>::value && table->maps_.size() != 0)
    {
      auto colMap = table->maps_[colNum];
      if (colMap->arrayMap_.size() != 0)
      {
        mapExists = colMap->arrayMap_[0]->GetMin(minVal);
      }
    }

    int64_t chunkNum = 0;
    rowId = 0;
    while (true)
    {

      std::shared_ptr<ArrayType> arr = std::static_pointer_cast<ArrayType>(chunkedArray->chunk(chunkNum));

      auto scanArray = [&]() -> bool
      {
        for (int64_t idx=0; idx<arr->length(); idx++)
        {
          if (value == arr->Value(idx))
          {
            return true;
          }
          rowId++;
        }
        return false;
      };

      if (mapExists)
      {
        auto arrMap = table->maps_[colNum]->arrayMap_[chunkNum];
        arrMap->GetMax(maxVal);
        arrMap->GetMin(minVal);
        if (value < minVal || value > maxVal)
        {
          rowId += arr->length();
        }
        else
        {
          int64_t tmpRowId;
          if (arrMap->GetRowId(tmpRowId, value))
          {
            rowId = tmpRowId;
            return true;
          }
        }
      }
      else
      {
        LOG(ERROR) << "No map here." ;
        if (scanArray())
          return true;
      }

      // Increase chunkNum
      chunkNum++;
      if (chunkNum >= chunkedArray->num_chunks())
        break;
    }

    return false;
  };

  // Get value from a rowId for a given chunkedArray
  template<class Type, class ArrayType>
  bool GetValue(int64_t& rowId,  // rowId input
                Type& value,      // output value
                std::shared_ptr<TTable> table, // table
                int64_t colNum)   // column number
  {
    std::shared_ptr<arrow::ChunkedArray> chunkedArray = table->table_->column(colNum);
    TColumnIterator<Type, ArrayType> columnIterator(chunkedArray);

    int64_t chunkRowId=0;
    for (int64_t chunkNum=0; chunkNum<=chunkedArray->num_chunks(); chunkNum++)
    {
      std::shared_ptr<ArrayType> array =
        std::static_pointer_cast<ArrayType>(chunkedArray->chunk(chunkNum));
      int64_t length = array->length();
      if (chunkRowId+length < rowId)
      {
        chunkRowId += length;
      }
      else
      {
        int64_t offset = rowId-chunkRowId;
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

// This performs the following operation
// 1. Find rowId where leftTable[leftColNum][rowId] == leftValue
// 2. Return rightValue equal to rightTable[rightColNum][rowId]
//
template<class Type, class ArrayType>
bool JoinInner(Type& leftValue,      // leftValue Input
               std::shared_ptr<TTable> leftTable, // leftTable
               int64_t leftColNum,      // leftColNum
               int64_t& leftRowIdInMicroseconds,   // time taken to look for leftValue
               Type& rightValue,    // rightValue Result
               std::shared_ptr<TTable> rightTable, // right Table
               int64_t rightColNum,     // right Col Num
               int64_t& rightValueInMicroseconds)  // time taken to look for rightValue
{

  int64_t rowId;
  StopWatch timer;
  timer.Start();
  bool result = GetRowId<Type, ArrayType>(rowId, leftValue, leftTable, leftColNum);
  timer.Stop();
  leftRowIdInMicroseconds += timer.ElapsedInMicroseconds();
  if (!result)
  {
    return result;
  }

  timer.Start();
  result = GetValue<Type, ArrayType>(rowId, rightValue, rightTable, rightColNum);
  timer.Stop();
  rightValueInMicroseconds += timer.ElapsedInMicroseconds();

  return result;
}

};
