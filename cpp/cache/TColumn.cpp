#include <TColumn.h>
#include <TCatalog.h>
#include <TBlock.h>
#include <TColumnMap.h>

namespace liten
{

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


}
