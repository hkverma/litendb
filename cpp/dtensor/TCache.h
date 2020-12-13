//
// TenDB Columnar Cache Directory
//
// All tables are held in TCache.
// Each table when read in cached with a unique UUID.
//
// For now all components are received from here and not stored locally.
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

#include <unordered_map>
#include <boost/uuid/uuid.hpp>
#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <arrow/api.h>
#include <arrow/csv/api.h>
#include "TTable.h"

#pragma once
//
// Cache a ttable some
//  remote storage, local storage, in-memory (that is cache hierarchy)
//
//
// provide hash template with uuid
//
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

// TCache with tables
namespace tendb {

  class TCache {
  public:

    /// Get a singleton instance, if not present create one
    static std::shared_ptr<TCache> GetInstance(bool& newInst);

    /// Read csv file for now
    std::shared_ptr<TTable> ReadCsv
      (std::string tableName,
       std::string csvFileName,
       const arrow::csv::ReadOptions& read_options = arrow::csv::ReadOptions::Defaults(),
       const arrow::csv::ParseOptions& parse_options = arrow::csv::ParseOptions::Defaults(),
       const arrow::csv::ConvertOptions& convert_options = arrow::csv::ConvertOptions::Defaults());
    
    /// Get Table from Id
    std::shared_ptr<TTable> GetTable(boost::uuids::uuid id);
    /// Get Table from tableName
    std::shared_ptr<TTable> GetTable(std::string tableName);

    /// Add table to cache
    int AddTable(std::shared_ptr<TTable> ttable);

  private:

    /// Get table from table uuid
    std::unordered_map<boost::uuids::uuid, std::shared_ptr<TTable>> tables_;
    /// map table name to an
    std::unordered_map<std::string, boost::uuids::uuid> cacheIds_;
    /// Generate a new ID
    // TODO move to a single uuid util function
    boost::uuids::random_generator idGenerator;
    /// Get Id for the given 
    bool GetId(std::string csvFileName, boost::uuids::uuid& cacheId);
    
    /// Cache keeps all the tables
    static std::shared_ptr<TCache> tCache_;
    
  };

};

// These functions are exposed for external python like bindings
extern "C"
{
  tendb::TCache* TCache_GetInstance();
  int TCache_AddTable(tendb::TCache *tcache, char* name, void* table);
}

