//
// TenDB Columnar Cache Directory
//
// Fetch from the cache and store here
// For now all components are received from here and not stored locally
//
// TODO
//  Cache mechanisms -
//     LRU     - Least Recently Used
//     LRU (T) - Least Recently Used with timeout
//     TRU     - Timed recently used
//
//  Cache Async event broadcasts
//    Before evicting a cache broadcast and sync with all the data components
//    who have a copy. 
//
//  Columnar Data storage
//
//   One column chunk stores numComponents of a single column of TenDB
//   Arrow in memory, persistent in parquet format
//
#pragma once

#include <unordered_map>
#include <boost/uuid/uuid.hpp>
#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <arrow/api.h>
#include "TTable.h"

//
// Cache a ttable some
//  remote storage, local storage, in-memory (that is cache hierarchy)
//
//
// provide hash template with uuid
namespace std
{
  template<>
  struct hash<boost::uuids::uuid>
  {
    size_t operator () (const boost::uuids::uuid& uid) const
    {
        return boost::hash<boost::uuids::uuid>()(uid);
    }
  };
  
};

// TCache with hash templates
namespace tendb {

  class TCache {
  public:

    static std::shared_ptr<TCache> GetInstance();
    static std::shared_ptr<TCache> tCache_;

    std::shared_ptr<TTable> Read(std::string csvFileName);

    std::shared_ptr<TTable> GetTable(boost::uuids::uuid id);
    bool GetId(std::string csvFileName, boost::uuids::uuid& cacheId);

    // map uuid to cache Table pointer
    std::unordered_map<boost::uuids::uuid, std::shared_ptr<TTable>> tables_;
    // map file name to an Id
    std::unordered_map<std::string, boost::uuids::uuid> cacheIds_;
    boost::uuids::random_generator idGenerator;

  };

};
