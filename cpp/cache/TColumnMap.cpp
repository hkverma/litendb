#include <TColumn.h>
#include <TColumnMap.h>
// TBD All ValueOrDie type returns should be made macros
namespace liten
{

// TODO - Split zone maps from inverted index
//
// Create maps for a given chunkedArray
//
// Need builder for each type
// go through the list and collect min & max
// arrow::builder.cc check L22 for the types shown below
//
TResult<std::shared_ptr<TColumnMap>> TColumnMap::Create(std::shared_ptr<TColumn> tColumn)
{
  if (0 == tColumn->NumBlocks())
  {
    return TStatus::Invalid("Empty Columnar data");
  }
  
  std::shared_ptr<TColumnMap> colMap = std::make_shared<TColumnMap>(tColumn);

  auto idType = tColumn->GetBlock(0)->GetArray()->type()->id();
  for (auto i=0; i<tColumn->NumBlocks(); i++)
  {
    if (idType != tColumn->GetBlock(i)->GetArray()->type()->id())
    {
      return TStatus::Invalid("Different type of data in the same column");
    }
  }
  switch (idType)
  {
    /* TBD
       case UInt8: case Int8: case UInt16:  case Int16:
       case UInt32:
       case Int32:
       case UInt64:
    */
  case arrow::Int64Type::type_id:
    {
      auto colMapResult = TInt64ColumnMap::Create(tColumn);
      if (!colMapResult.ok())
      {
        return colMapResult.status();
      }
      colMap = colMapResult.ValueOrDie();
      
      break;
    }
    /* TBD
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
      break ;
    }
  }
  tColumn->SetMap(colMap);
  return colMap;
}

TInt64ColumnMap::TInt64ColumnMap(std::shared_ptr<TColumn> tColumn)
  : TColumnMap(tColumn)
{
  min_.resize(tColumn->NumBlocks(), std::numeric_limits<int64_t>::min());
  max_.resize(tColumn->NumBlocks(), std::numeric_limits<int64_t>::max());
}

TResult<std::shared_ptr<TInt64ColumnMap>> TInt64ColumnMap::Create(std::shared_ptr<TColumn> tColumn)
{
  std::shared_ptr<TInt64ColumnMap> mapArr = std::make_shared<TInt64ColumnMap>(tColumn);
  for (int64_t blkNum=0; blkNum<tColumn->NumBlocks(); blkNum++)
  {
    int64_t& minVal = mapArr->min_[blkNum];
    int64_t& maxVal = mapArr->max_[blkNum];
    std::shared_ptr<arrow::Array> arr = tColumn->GetBlock(blkNum)->GetArray();

    std::shared_ptr<arrow::Int64Array> numArr = std::static_pointer_cast<arrow::Int64Array>(arr);
    if (numArr == nullptr)
    {
      LOG(ERROR) << "Internal error - cannot convert to Int64Array." ;
      // TBD return internal error result
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
      mapArr->reverseMap_.insert(std::make_pair(rowVal, std::make_pair(blkNum, rowId)));
    }
  }
  return mapArr;
}

bool TInt64ColumnMap::GetReverseMap(int64_t& rowVal, int64_t& arrId, int64_t& rowId)
{
  auto revMapItr = reverseMap_.find(rowVal);
  if (revMapItr == reverseMap_.end())
  {
    return false;
  }
  // For now return only one
  // TBD Use equal_range in future to return a vector of matched rowIds
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

}
