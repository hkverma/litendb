//#include <iostream>
#include <common.h>

#include <TBlock.h>
#include <TCache.h>
#include <TCatalog.h>

using namespace liten;

// For now the columnChunk should be present here.
// TBD In future it could not be here, in that case fetch it if not present here
//      Use status code to return back instance
//      Use arrow::Result type object for getting value

/// Singleton cache instance
std::shared_ptr<TCache> TCache::tCache_ = nullptr;

// Get a singleton instance, if not present create one
std::shared_ptr<TCache> TCache::GetInstance()
{
  if (nullptr == tCache_)
  {
    tCache_ = std::make_shared<TCache>();
  }
  TLOG(INFO) << "Created a new TCache";
  return tCache_;
}

// Get Cache information
std::string TCache::GetInfo()
{
  std::stringstream ss;
  ss << "{\n";
  ss << TConfigs::GetInstance()->GetComputeInfo();
  ss << "\n}\n,{\n";
  ss << TCatalog::GetInstance()->GetTableInfo();
  ss << "\n}";
  TLog::GetInstance()->FlushLogFiles(TLog::Info);
  return std::move(ss.str());
}


