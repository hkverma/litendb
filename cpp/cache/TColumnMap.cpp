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
TResult<std::shared_ptr<TColumnMap>> TColumnMap::Create(std::shared_ptr<TColumn> tColumn,
                                                        bool ifZoneMap,
                                                        bool ifReverseMap)
{
  if (0 == tColumn->NumBlocks())
  {
    return TStatus::Invalid("Empty Columnar data");
  }
  
  std::shared_ptr<TColumnMap> colMap = std::make_shared<TColumnMap>(tColumn, ifZoneMap, ifReverseMap);

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
      auto colMapResult = TInt64ColumnMap::Create(tColumn, ifZoneMap, ifReverseMap);
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

TInt64ColumnMap::TInt64ColumnMap(std::shared_ptr<TColumn> tColumn, bool ifZoneMap, bool ifReverseMap)
  : TColumnMap(tColumn, ifZoneMap, ifReverseMap)
{
  min_.resize(tColumn->NumBlocks(), std::numeric_limits<int64_t>::min());
  max_.resize(tColumn->NumBlocks(), std::numeric_limits<int64_t>::max());
}

TResult<std::shared_ptr<TInt64ColumnMap>> TInt64ColumnMap::Create(std::shared_ptr<TColumn> tColumn,
                                                                  bool ifZoneMap,
                                                                  bool ifReverseMap)
{
  std::shared_ptr<TInt64ColumnMap> mapArr = std::make_shared<TInt64ColumnMap>(tColumn, ifZoneMap, ifReverseMap);
  if (mapArr->ifZoneMap_)
  {
    TStatus status = mapArr->CreateZoneMap();
    LITEN_RETURN_IF(!status.ok(), status);
  }
  if (mapArr->ifReverseMap_)
  {
    TStatus status = mapArr->CreateReverseMap();
    LITEN_RETURN_IF(!status.ok(), status);
  }
  return mapArr;
}

TStatus TInt64ColumnMap::CreateZoneMap()
{
  for (int64_t blkNum=0; blkNum<tColumn_->NumBlocks(); blkNum++)
  {
    int64_t& minVal = min_[blkNum];
    int64_t& maxVal = max_[blkNum];
    std::shared_ptr<arrow::Array> arr = tColumn_->GetBlock(blkNum)->GetArray();

    std::shared_ptr<arrow::Int64Array> numArr = std::static_pointer_cast<arrow::Int64Array>(arr);
    if (numArr == nullptr)
    {
      return TStatus::UnknownError("Cannot convert to Int64Array.");
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
    }
  }
  return TStatus::OK();
}

TStatus TInt64ColumnMap::CreateReverseMap()
{
  for (int64_t blkNum=0; blkNum<tColumn_->NumBlocks(); blkNum++)
  {
    std::shared_ptr<arrow::Array> arr = tColumn_->GetBlock(blkNum)->GetArray();
    std::shared_ptr<arrow::Int64Array> numArr = std::static_pointer_cast<arrow::Int64Array>(arr);
    if (numArr == nullptr)
    {
      return TStatus::UnknownError("cannot convert to Int64Array.");
    }

    int64_t length = numArr->length();
    for (int64_t rowId=0 ; rowId<length; rowId++)
    {
      int64_t rowVal = numArr->Value(rowId);
      reverseMap_.insert(std::make_pair(rowVal, std::make_pair(blkNum, rowId)));
    }
  }
  return TStatus::OK();
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
