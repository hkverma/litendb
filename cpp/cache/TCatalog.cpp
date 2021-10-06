#include <TCatalog.h>
#include <TTable.h>
#include <TSchema.h>

namespace liten
{

std::shared_ptr<TCatalog> TCatalog::tCatalog_=nullptr;

// Get a singleton instance, if not present create one
std::shared_ptr<TCatalog> TCatalog::GetInstance()
{
  if (nullptr == tCatalog_)
  {
    tCatalog_ = std::make_shared<TCatalog::MakeSharedEnabler>();
  }
  TLOG(INFO) << "Created a new TCatalog";
  return tCatalog_;
}

// Add Block to Cache
TStatus TCatalog::AddBlock(std::shared_ptr<TBlock> block,
                           TGuid::Uuid& id)
{

  if (nullptr == block)
  {
    return TStatus::KeyError("Catalog cannot add null Block");
    return TStatus::OK();
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

  return TStatus::OK();
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
  {
    std::shared_lock<std::shared_mutex> lk(mutex_);
    bool firstTable = true;
    for (auto& tableId : tables_)
    {
      if (firstTable) {
        firstTable = false;
      } else {
        str.append(",");
      }
      std::string tableName = tableId.first;
      auto tTable = tableId.second;
      str.append("\"").append(tableName).append("\"");
      str.append(":\"").append(TableTypeString[tTable->GetType()]).append("\"");
    }
  }
  str.append("}");
  return std::move(str);
}

/// Get Table by tableName
std::shared_ptr<TTable> TCatalog::GetTable(std::string tableName) const
{
  std::shared_lock<std::shared_mutex> lk(mutex_);
  auto itr = tables_.find(tableName);
  if (tables_.end() == itr)
    return nullptr;
  return (itr->second);
}

// Add a table to catalog, should be added after all blocks have been added
TStatus TCatalog::AddTable(std::shared_ptr<TTable> ttable)
{
  std::string tableName = std::move(ttable->GetName());
  std::unique_lock<std::shared_mutex> lk(mutex_);
  auto itr = tables_.find(tableName);
  if (tables_.end() != itr)
  {
    if (itr->second == ttable)
    {
      TLOG(INFO) << "Table=" << tableName << " is already in catalog";
      return TStatus::OK();
    }
    else
    {
      return TStatus::AlreadyExists("Modifying existing Table name=", tableName, " with a different liten table");
    }
  }
  tables_[tableName] = ttable;
  return TStatus::OK();
}

// Return information with compute information
// TBD Have to use catalog here ...
// TBD shared_lock ???
std::string TCatalog::GetSchemaInfo() const
{
  std::string str;
  str.append("{");
  {
    bool firstTable = true;
    std::shared_lock<std::shared_mutex> lk(mutex_);
    for (auto& schemaId : schemas_)
    {
      if (firstTable) {
        firstTable = false;
      } else {
        str.append(",");
      }
      std::string schemaName = schemaId.first;
      auto tSchema = schemaId.second;
      str.append("\"").append(schemaName).append("\"");
      str.append(":\"").append(TableTypeString[tSchema->GetType()]).append("\"");
    }
  }
  str.append("}");
  return std::move(str);
}

/// Get Schema by schemaName
std::shared_ptr<TSchema> TCatalog::GetSchema(std::string schemaName) const
{
  std::shared_lock<std::shared_mutex> lk(mutex_);
  auto itr = schemas_.find(schemaName);
  if (schemas_.end() == itr)
    return nullptr;
  return (itr->second);
}

// Add a schema to catalog, should be added after all blocks have been added
TStatus TCatalog::AddSchema(std::shared_ptr<TSchema> tschema)
{
  std::unique_lock<std::shared_mutex> lk(mutex_);
  std::string schemaName = std::move(tschema->GetName());
  auto itr = schemas_.find(schemaName);
  if (schemas_.end() != itr)
  {
    if (itr->second == tschema)
    {
      TLOG(INFO) << "Schema=" << schemaName << " is already in catalog";
      return TStatus::OK();
    }
    else
    {
      return TStatus::AlreadyExists("Modifying existing Schema name=", schemaName, " with a different liten schema");
    }
  }
  schemas_[schemaName] = tschema;
  return TStatus::OK();
}

TStatus TCatalog::AddSchemaForTable(std::string schemaName, std::string tableName)
{
  std::unique_lock<std::shared_mutex> lk(mutex_);
  auto itr = schemaToTables_.find(schemaName);
  if (schemaToTables_.end() != itr)
  {
    if (tableName.compare(itr->second))
    {
      return TStatus::AlreadyExists("Only one table allowed per Schema name=", schemaName, " table=", tableName);
    }
    else
    {
      TLOG(INFO) << "Schema=" << schemaName << " already has table=" << tableName;
      return TStatus::OK();
    }
  }
  schemaToTables_[schemaName] = std::move(tableName);
  return TStatus::OK();
}

}
