//
// Liten Columnar Cache Directory
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
//   One column chunk stores numComponents of a single column of Liten
//   Arrow in memory, persistent in parquet format
//
// TODO
//   Add and remove tables, clear cache commands needed
//
#include <unordered_map>
#include <common.h>

#include <arrow/api.h>
#include <arrow/csv/api.h>
#include "TTable.h"

#pragma once
//
// Cache a ttable some
//  remote storage, local storage, in-memory (that is cache hierarchy)

namespace liten {

  /// TCache maintains a Liten cache
  class TCache {
  public:

    /// Get a singleton instance, if not present create one
    static std::shared_ptr<TCache> GetInstance();
    
    /// Get Cache info
    std::string GetInfo();
    
    /// Read csv file for now
    std::shared_ptr<TTable> ReadCsv
      (std::string tableName,
       std::string csvFileName,
       const arrow::csv::ReadOptions& read_options = arrow::csv::ReadOptions::Defaults(),
       const arrow::csv::ParseOptions& parse_options = arrow::csv::ParseOptions::Defaults(),
       const arrow::csv::ConvertOptions& convert_options = arrow::csv::ConvertOptions::Defaults());

    /// Read csv file in an arrow table
    std::shared_ptr<arrow::Table> ReadCsv
      (std::string csvFileName,
       const arrow::csv::ReadOptions& readOptions,
       const arrow::csv::ParseOptions& parseOptions,
       const arrow::csv::ConvertOptions& convertOptions);
    
    /// Get Table from Id
    std::shared_ptr<TTable> GetTable(boost::uuids::uuid id);
    /// Get Table from tableName
    std::shared_ptr<TTable> GetTable(std::string tableName);

    /// Make maps for a given table name
    int MakeMaps(std::string tableName);
    int MakeMaps(std::shared_ptr<TTable> ttable);
    
    /// Make maps for all dimension tables
    int MakeMaps();

    /// Add table to cache
    std::shared_ptr<TTable> AddTable(std::string name,
                                     std::shared_ptr<arrow::Table> table,
                                     TTable::TType type);

    // Define various cuts here - slicing and dicing
    // Examples are - point, range, set

    // Defining first cut here based on index, it creates a new arrow table
    // https://cubes.readthedocs.io/en/v1.0.1/slicing_and_dicing.html

    // This gives a slice from offset from beginning of length length
    std::shared_ptr<arrow::Table> Slice(std::string tableName, int64_t offset, int64_t length);

    using VersionToUuidMap = std::map<int64_t, boost::uuids::uuid>;
    using TableNameColumnNamePair = std::pair<std:;string, std::string>;
    
  private:

    /// Map an array uuid to an array pointer
    std::unordered_map<boost::uuids::uuid, std::shared_ptr<TBlock>> blocks_;
    
    /// map table name and field_name to a map containing pairs of version and array UUIDs
    std::unordered_map<TableNameColumnNamePair, VersionToUuidMap> blockIds_;
    
    /// Get Id for given table name and field name
    /// @param blockName table and column name
    /// @param cacheId UUid for the given pair
    bool GetId(TableNameColumnNamePair blockName, boost::uuids::uuid& cacheId);

    /// hash map table name to its URI
    std::unordered_map<string tableName, string tableUri> tablesUri_;
    
    /// A singleton cache keeps all the tables
    static std::shared_ptr<TCache> tCache_;

    /// CacheConfig 
  };

};
