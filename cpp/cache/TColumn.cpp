#include <TColumn.h>
#include <TCatalog.h>
#include <TBlock.h>
#include <TColumnMap.h>

namespace liten
{

TRowId TColumn::GetRowId(int64_t rowNum)
{
  TRowId id;
  id.blkNum = -1;
  int64_t blkId = 0;
  for (int64_t blkNum=0; blkNum<=blocks_.size(); blkNum++)
  {
    int64_t length = blocks_[blkNum]->GetArray()->length();
    if (blkId+length < rowNum)
    {
      blkId += length;
    }
    else
    {
      id.blkNum = blkNum;
      id.rowNum = rowNum-blkId;
      break;
    }
  }
  return id;
}

TStatus TColumn::CreateZoneMap(bool forceCreate)
{
  return std::move(map_->CreateZoneMap());
}

TStatus TColumn::CreateReverseMap(bool forceCreate)
{
  return std::move(map_->CreateReverseMap());
}

TStatus TColumn::Add(std::shared_ptr<TBlock> tBlock)
{
  if (nullptr == tBlock)
  {
    return TStatus::Invalid("Cannot add null block to column");
  }
  blocks_.push_back(tBlock);
  numRows_ += tBlock->GetArray()->length();
  return TStatus::OK();
}

TResult<std::shared_ptr<TColumnMap>> TColumn::GetMap()
{
  // If map already exists, return it
  if (nullptr != map_)
  {
    return map_;
  }
  
  // Create a new one 
  auto colMapResult = std::move(TColumnMap::Create(shared_from_this()));
  if (!colMapResult.status().ok())
    return colMapResult;
  map_ = colMapResult.ValueOrDie();
  return colMapResult;
  
}

std::shared_ptr<arrow::ChunkedArray> TColumn::Slice(int64_t offset, int64_t length)
{
  if (offset >= numRows_)
  {
    TLOG(INFO) << "Slice offset greater than array length";
    return nullptr;
  }
  // TBD make it data member of TColumn
  std::shared_ptr<arrow::DataType> type;
  bool offset_equals_length = (offset == numRows_);
  int64_t currBlock = 0;
  while (currBlock < NumBlocks() && offset >= GetBlock(currBlock)->GetArray()->length())
  {
    offset -= GetBlock(currBlock)->GetArray()->length();
    currBlock++;
  }

  arrow::ArrayVector newChunks;
  if (NumBlocks() > 0 && (offset_equals_length || length == 0))
  {
    // Special case the zero-length slice to make sure there is at least 1 Array
    // in the result. When there are zero chunks we return zero chunks
    newChunks.push_back(GetBlock(std::min(currBlock, NumBlocks() - 1))->GetArray()->Slice(0, 0));
    type = GetBlock(std::min(currBlock, NumBlocks() - 1))->GetArray()->type();
  }
  else
  {
    while (currBlock < NumBlocks() && length > 0) {
      newChunks.push_back(GetBlock(currBlock)->GetArray()->Slice(offset, length));
      length -= GetBlock(currBlock)->GetArray()->length() - offset;
      type = GetBlock(currBlock)->GetArray()->type();
      offset = 0;
      currBlock++;
    }
  }

  return std::make_shared<arrow::ChunkedArray>(newChunks, type);
}

arrow::Result<std::shared_ptr<arrow::Scalar>> TColumn::GetScalar(int64_t rowId)
{
  int64_t blkId=0;
  arrow::Result<std::shared_ptr<arrow::Scalar>> result;
  for (int64_t blkNum=0; blkNum<blocks_.size(); blkNum++)
  {
    auto arr = blocks_[blkNum]->GetArray();
    int64_t length = arr->length();
    if (blkId+length < rowId)
    {
      blkId += length;
    }
    else
    {
      int64_t offset = rowId-blkId;
      result = arr->GetScalar(offset);
    }
  }
  return result;  
}

arrow::Result<std::shared_ptr<arrow::Scalar>> TColumn::GetScalar(int64_t arrId, int64_t rowId)
{
  int64_t blkId=0;
  arrow::Result<std::shared_ptr<arrow::Scalar>> result;
  if (arrId>=0 && arrId<blocks_.size())
  {
    auto arr = blocks_[arrId]->GetArray();
    result = arr->GetScalar(rowId);
  }
  return result;  
}

std::string TColumn::ToString(bool values, bool zonemap, bool reversemap)
{
  std::stringstream ss;
  ss << GetName() << "=";
  if (values)
  {
    ss << "\nValues=";
    for (auto bn=0; bn<blocks_.size(); bn++)
    {
      ss << "Block " << bn << "=" << std::move(blocks_[bn]->ToString()) << ";";
    }
  }
  
  if (zonemap)
  {
    ss << "\nZoneMap=";
    for (int bn = 0; bn<NumBlocks(); bn++)
    {
      int64_t minVal, maxVal;
      auto arr = GetBlock(bn)->GetArray();
      ss << " Arr " << bn << " Size=" << arr->length();
      ss << " Type=" << arr->type()->ToString() ;
      ss << " Min=";
      map_->GetMin(bn,minVal)?(ss << minVal):(ss << "None");
      ss << " Max=";
      map_->GetMax(bn,maxVal)?(ss << maxVal):(ss << "None");
      ss << ";" ;
    }
  }
  if (reversemap)
  {
    ss << "\nReverseMap=";
    map_->GetReverseMap(ss);
    ss << "; ";
  }
  return (std::move(ss.str()));
}

int64_t TColumn::GetRowNum(TRowId rowId)
{
  int64_t rowNum = rowId.rowNum;
  for (auto blkId=0; blkId <rowId.blkNum; blkId++)
  {
    rowNum += blocks_[blkId]->GetArray()->length();
  }
  return rowNum;
}

}
