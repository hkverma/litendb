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

TStatus TColumn::CreateMap()
{
  auto colMapResult = TColumnMap::Create(shared_from_this());
  return colMapResult.status();
}

}
