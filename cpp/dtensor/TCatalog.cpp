#include <dtensor.h>

using namespace liten;

std::shared_ptr<TCatalog> TCatalog::tCatalog_=nullptr;

// Add Block to Cache  
Status TCatalog::AddBlock(std::shared_ptr<TBlock> block,
                          Tguid::Uuid& id)
{
  
  if (nullptr == block)
  {
    return Status::KeyError("Catalog cannot add null Block");    
    return Status::OK();
  }

  // Now add the block within lock
  {
    std::unique_lock<std::shared_mutex> lk(mutex_);
    auto it = blockToId_.find(block);
    if (blockToId_.end() != it) {
      return Status::KeyError("Block already exists in catalog");
    }
    Tguid::Uuid id = Tguid::GetInstance()->GenerateUuid();
    idToBlock_[id] = block;
    blockToId_[block] = id;
  }
  
  return Status::OK();
}

// Return true if block exists
bool TCatalog::IfExists(std::shared_ptr<TBlock> block,
                        Tguid::Uuid& id)
{
  std::shared_lock<std::shared_mutex> lk(mutex_);
  auto it = blockToId_.find(block);
  if (blockToId_.end() == it) {
    return false;
  }
  id = it->second;
  return true;
}
