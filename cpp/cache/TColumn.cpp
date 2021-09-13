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

TStatus TColumn::CreateMap(bool zoneMap, bool reverseMap)
{
  auto colMapResult = std::move(TColumnMap::Create(shared_from_this()));
  LITEN_RETURN_IF(!colMapResult.status().ok(), colMapResult.status());
  auto colMap = colMapResult.ValueOrDie();
  TStatus status;
  if (zoneMap)
  {
    status = colMap->CreateZoneMap();
  }
  if (reverseMap)
  {
    status = colMap->CreateReverseMap(); 
  }
  return status;
}

}
