//
// TenDB Columnar Storage Node
//
// Columnar Data storage
//
// One column chunk stores numComponents of a single column of TenDB
// Arrow in memory, persistent in parquet format
//
#pragma once

#include <boost/uuid/uuid.hpp>
#include <arrow/api.h>
#include <vector>

namespace tendb {

  // TODO Use Status to return value wrapped with a message

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

  template<class Type, class ArrayType>
  bool GetRowId(int64_t& rowId, Type& value, std::shared_ptr<arrow::ChunkedArray> chunkedArray)
  {
    TColumnIterator<Type, ArrayType> columnIterator(chunkedArray);
    Type currentValue;
    rowId = 0;
    while (columnIterator.next(currentValue))
    {
      if (currentValue == value)
      {
        return true;
      }
      rowId++;
    }
    return false;
  };

  template<class Type, class ArrayType>
  bool GetValue(int64_t& rowId, Type& value, std::shared_ptr<arrow::ChunkedArray> chunkedArray)
  {
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
  
  // TColumns can be a transformation of arrow::ChunkedArray
  template <class Type, class ArrayType>  
  class TColumn {
  public:
    TColumn(std::shared_ptr<arrow::ChunkedArray> chary) :
      chunkedArray_(chary) { }

    std::shared_ptr<arrow::ChunkedArray> chunkedArray_;
    std::shared_ptr<arrow::ChunkedArray> transformedChunkedArray_;
  };
};
