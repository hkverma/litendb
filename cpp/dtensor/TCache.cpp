#include "TCache.h"
#include <iostream>
#include <common.h>

#include <boost/uuid/random_generator.hpp>

using namespace tendb;

// For now the columnChunk should be present here.
// TODO In future it could not be here, in that case fetch it if not present here

/// Singleton cache instance
std::shared_ptr<TCache> TCache::tCache_ = nullptr;

/// Get a singleton instance, if not present create one
// TODO Use status code to return back instance

/// Read csv file in a new table tableName. tableName should be unique
std::shared_ptr<TTable> TCache::ReadCsv
(std::string tableName,
 std::string csvFileName,
 const arrow::csv::ReadOptions& readOptions,
 const arrow::csv::ParseOptions& parseOptions,
 const arrow::csv::ConvertOptions& convertOptions)
{
  // If found one, return it
  std::shared_ptr<TTable> table = GetTable(tableName);
  if (nullptr != table)
  {
    // TODO log here or add csv to the same table
    return table;
  }

  // Create one table with tableName
  boost::uuids::uuid cacheId;
  table = std::make_shared<TTable>(tableName);
  if (table->ReadCsv(csvFileName, readOptions, parseOptions, convertOptions))
  {
    boost::uuids::uuid cacheId = idGenerator();
    tables_[cacheId] = table;
    cacheIds_[tableName] = cacheId;
  }
  else
  {
    table = nullptr;
  }

  return table;
}

/// Get Table from the cache
std::shared_ptr<TTable> TCache::GetTable(boost::uuids::uuid id)
{
  auto itr = tables_.find(id);
  if (itr ==  tables_.end())
    return nullptr;
  return itr->second;
}

/// GetTable from table name
std::shared_ptr<TTable> TCache::GetTable(std::string tableName)
{
  boost::uuids::uuid cacheId;
  if (GetId(tableName, cacheId))
  {
    return GetTable(cacheId);
  }
  return nullptr;
}

// TODO use arrow::Result type object here
// use std::move for all pass by value
bool TCache::GetId(std::string tableName, boost::uuids::uuid& cacheId)
{
  auto itr = cacheIds_.find(tableName);
  if (itr ==  cacheIds_.end())
    return false;
  cacheId = itr->second;
  return true;
}

/*  std::shared_ptr<arrow::Scalar> TCache::GetScalar(std::shared_ptr<arrow::Array> comp, int64_t rowNum)
    {
    arrow::Result<std::shared_ptr<arrow::Scalar>> compValResult = comp->GetScalar(rowNum);
    assert(compValResult.ok());
    std::shared_ptr<arrow::Scalar> compVal = compValResult.ValueOrDie();
    return compVal;
    }*/

int TCache::AddTable(std::shared_ptr<TTable> ttable)
{
  boost::uuids::uuid cacheId;
  std::string tableName = move(ttable->GetName());
  if (GetId(tableName, cacheId))
  {
    LOG(ERROR) << "Cannot add table " << tableName << ". It already exists";
    return 1;
  }
  cacheId = idGenerator();
  tables_[cacheId] = ttable;
  cacheIds_[tableName] = cacheId;
  return 0;
}

std::shared_ptr<TCache> TCache::GetInstance()
{
  if (tCache_ == nullptr)
  {
    tCache_ = std::make_shared<TCache>();
    google::InitGoogleLogging("tendb");
  }
  LOG(INFO) << "Created a new TCache";
  return tCache_;
}

// These functions are exposed for external python like bindings
TCache* TCache_GetInstance() {
  auto tcache = TCache::GetInstance();
  return tcache.get();
}

std::shared_ptr<TTable> TCache::AddTable(std::string name, std::shared_ptr<arrow::Table> table)
{
  boost::uuids::uuid cacheId;
  if (GetId(name, cacheId))
  {
    LOG(ERROR) << "Cannot add table " << name << ". It already exists";
    return nullptr;
  }
  
  LOG(INFO) << "Adding new table " << name;
  auto ttable = std::make_shared<TTable>(name, table);
  cacheId = idGenerator();
  tables_[cacheId] = ttable;
  cacheIds_[name] = cacheId;
  return ttable;
}  

// TODO how to send messages back to python notebook, using cython will make this redundant
int TCache_AddTable(TCache *tcache, char* name, void* table)
{
  std::shared_ptr<arrow::Table> tablePtr( (arrow::Table*) table);
  LOG(INFO) << "Adding new table " << name;
  LOG(INFO) << "Table ptr" << table;
  
  LOG(INFO) << "Table name " << tablePtr->field(0)->ToString() ;

  auto ttable = std::make_shared<TTable>(name, tablePtr);
  int status = tcache->AddTable(ttable);
  return status;

}
