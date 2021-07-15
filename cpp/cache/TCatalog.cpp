#include <TCatalog.h>
#include <TTable.h>

using namespace liten;

std::shared_ptr<TCatalog> TCatalog::tCatalog_=nullptr;

// Add Block to Cache  
Status TCatalog::AddBlock(std::shared_ptr<TBlock> block,
                          TGuid::Uuid& id)
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
      id = it->second;
    }
    else
    {
      TGuid::Uuid id = TGuid::GetInstance()->GenerateUuid();
      idToBlock_[id] = block;
      blockToId_[block] = id;
    }
  }
  
  return Status::OK();
}

// Return true if block exists
bool TCatalog::IfExists(std::shared_ptr<TBlock> block,
                        TGuid::Uuid& id)
{
  std::shared_lock<std::shared_mutex> lk(mutex_);
  auto it = blockToId_.find(block);
  if (blockToId_.end() == it) {
    return false;
  }
  id = it->second;
  return true;
}

// Return information with compute information
// TBD Have to use catalog here ...
// TBD shared_lock ???
std::string TCatalog::GetTableInfo() const
{
  std::string str;
  str.append("{");
  for (auto& tableId : tables_)
  {
    std::string tableName = tableId.first;
    auto tTable = tableId.second;
    if (DimensionTable == tTable->GetType()) {
      str.append("\"Dim\":");
    } else if (FactTable == tTable->GetType()) {
      str.append("\"Fact\":");
    } else {
      str.append("\"Unknown\":");
    }
    str.append("\"").append(tableName).append("\"");
    /*TBD  if (schema)
      ttable->PrintSchema();
      if (table)
      ttable->PrintTable(); */
  }
  str.append("}");
  return std::move(str);
}

/// Get Table by tableName
// TBD shared_lock ???
std::shared_ptr<TTable> TCatalog::GetTable(std::string tableName) const
{
  auto itr = tables_.find(tableName);
  if (tables_.end() == itr)
    return nullptr;
  return (itr->second);
}
