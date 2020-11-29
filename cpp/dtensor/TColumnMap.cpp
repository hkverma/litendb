#include "dtensor.h"
#include <iostream>

using namespace tendb;

// TODO - Split zone maps from inverted index
//
// Create maps for a given chunkedArray
//
// Need builder for each type
// go through the list and collect min & max
// arrow::builder.cc check L22 for the types shown below
//
std::shared_ptr<TColumnMap> TColumnMap::Make(std::shared_ptr<arrow::ChunkedArray> chunkedArray)
{
  std::shared_ptr<TColumnMap> chunkArrMap = std::make_shared<TColumnMap>(chunkedArray);

  switch (chunkedArray->type()->id())
  {
    /* TODO
       case UInt8: case Int8: case UInt16:  case Int16:
       case UInt32:
       case Int32:
       case UInt64:
    */
  case arrow::Int64Type::type_id:
    {
      chunkArrMap = TInt64ColumnMap::Make(chunkedArray);
      break;
    }
    /* TODO
       case Date32:
       case Date64:
       case Duration:
       case Time32:
       case Time64:
       case Timestamp:
       case MonthInterval:
       case DayTimeInterval:
       case Boolean:
       case HalfFloat:
       case Float:
       case Double:
       case String:
       case Binary:
       case LargeString:
       case LargeBinary:
       case FixedSizeBinary:
       case Decimal128:
    */
  default:
    {
      chunkArrMap = std::make_shared<TColumnMap>(chunkedArray);
      break ;
    }
  }
  return chunkArrMap;
}

std::shared_ptr<TColumnMap> TColumnMap::Copy()
{
  auto colMap = std::make_shared<TColumnMap>(chunkedArray_);
  return colMap;
}

TInt64ColumnMap::TInt64ColumnMap(std::shared_ptr<arrow::ChunkedArray> chunkedArray)
  : TColumnMap(chunkedArray)
{
  min_.resize(chunkedArray->num_chunks(), std::numeric_limits<int64_t>::min());
  max_.resize(chunkedArray->num_chunks(), std::numeric_limits<int64_t>::max());
}

std::shared_ptr<TInt64ColumnMap> TInt64ColumnMap::Make(std::shared_ptr<arrow::ChunkedArray> chunkedArray)
{
  std::shared_ptr<TInt64ColumnMap> mapArr = std::make_shared<TInt64ColumnMap>(chunkedArray);
  for (int64_t arrNum=0; arrNum<chunkedArray->num_chunks(); arrNum++)
  {
    int64_t& minVal = mapArr->min_[arrNum];
    int64_t& maxVal = mapArr->max_[arrNum];
    std::shared_ptr<arrow::Array> arr = chunkedArray->chunk(arrNum);

    std::shared_ptr<arrow::Int64Array> numArr = std::static_pointer_cast<arrow::Int64Array>(arr);
    if (numArr == nullptr)
    {
      LOG(ERROR) << "Internal error - cannot convert to Int64Array." ;
      continue;
    }

    int64_t length = numArr->length();
    // Initialize with the first value
    if (length >= 1)
    {
      minVal = numArr->Value(0);
      maxVal = numArr->Value(0);
    }
    for (int64_t rowId=0 ; rowId<length; rowId++)
    {
      int64_t rowVal = numArr->Value(rowId);
      // Set min and max
      minVal = (rowVal < minVal)?rowVal:minVal;
      maxVal = (rowVal > maxVal)?rowVal:maxVal;
      // Create an inverted index
      mapArr->reverseMap_.insert(std::make_pair(rowVal, std::make_pair(arrNum, rowId)));
    }
  }
  return mapArr;
}

std::shared_ptr<TColumnMap> TInt64ColumnMap::Copy()
{
  std::shared_ptr<TInt64ColumnMap> colMap = std::make_shared<TInt64ColumnMap>(chunkedArray_);
  colMap->min_ = min_;
  colMap->max_ = max_;
  colMap->reverseMap_ = reverseMap_;
  return colMap;
}

bool TInt64ColumnMap::GetReverseMap(int64_t& rowVal, int64_t& arrId, int64_t& rowId)
{
  auto revMapItr = reverseMap_.find(rowVal);
  if (revMapItr == reverseMap_.end())
  {
    return false;
  }
  // For now return only one
  // TODO Use equal_range in future to return a vector of matched rowIds
  arrId = (revMapItr->second).first;
  rowId = (revMapItr->second).second;
  return true;
}

bool TInt64ColumnMap::GetReverseMap(std::stringstream& ss)
{
  for (auto it = reverseMap_.begin(); it != reverseMap_.end(); it++)
  {
    ss << it->first << "=" << (it->second).first << ":" << (it->second).second << "," ;
  }
  return true;
}
