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

  template <class Type, class TypeArray>
  class TColumnIterator
  {
  public:
    TColumnIterator(std::shared_ptr<arrow::ChunkedArray> chary) :
      currentArrayRowId_(0), lastArrayRowId_(0), chunkNum_(0)
    {
      chunkedArray_ = chary;
      array_ =
        std::static_pointer_cast<TypeArray>(chunkedArray_->chunk(chunkNum_));
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
        std::static_pointer_cast<TypeArray>(chunkedArray_->chunk(chunkNum_));
      return true;
    }

    int64_t currentArrayRowId_;
    int64_t lastArrayRowId_;
    int64_t chunkNum_;
    std::shared_ptr<TypeArray> array_;
    std::shared_ptr<arrow::ChunkedArray> chunkedArray_;
  };
  
  class TColumn {
  public:

    std::shared_ptr<TColumnChunk> GetColumnChunk(int64_t rowNum);
    uint32_t columnNum_;
    uint32_t startOffset;

    arrow::Type::type type_;
    uint32_t totalChunks_;
    uint32_t totalComponents_;

    std::vector<int64_t> startOffsets_;
    std::vector<std::shared_ptr<TColumnChunk>> columnChunks_;

  };
};
