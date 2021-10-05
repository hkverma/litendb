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

}
