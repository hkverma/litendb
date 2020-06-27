#include "TCache.h"
#include <iostream>
#include <boost/uuid/random_generator.hpp>

namespace tendb {

  // For now the columnChunk should be present here. 
  // TODO In future it could not be here, in that case fetch it if not present here

  std::shared_ptr<TCache> TCache::tCache_;
  
  std::shared_ptr<TCache> TCache::GetInstance()
  {
    if (tCache_ == nullptr)
      tCache_ = std::make_shared<TCache>();
    return tCache_;
  }

  // Use status here
  std::shared_ptr<TTable> TCache::Read(std::string csvFileName)
  {
    std::shared_ptr<TTable> table = nullptr;
    boost::uuids::uuid cacheId;
    if ( !GetId(csvFileName, cacheId))
    {
      table = std::make_shared<TTable>();
      if (table->Read(csvFileName))
      {
        boost::uuids::uuid cacheId = idGenerator();
        tables_[cacheId] = table;
        cacheIds_[csvFileName] = cacheId;
      }
      else
      {
        table = nullptr;
      }
    }
    else
    {
      table = GetTable(cacheId);
    }
    return table;
  }
  
  // TODO GetArray should fetch
  std::shared_ptr<TTable> TCache::GetTable(boost::uuids::uuid id)
  {
    auto itr = tables_.find(id);
    if (itr ==  tables_.end())
      return nullptr;
    return itr->second;
  }

  // TODO use arrow::Result type object here
  // use std::move for all pass by value
  bool TCache::GetId(std::string csvFileName, boost::uuids::uuid& cacheId) 
  {
    auto itr = cacheIds_.find(csvFileName);
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

}
