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

#pragma once
#include <common.h>

namespace liten {

  /// TCache maintains a Liten cache
  class TCache {
  public:

    /// Get a singleton instance, if not present create one
    static std::shared_ptr<TCache> GetInstance();
    
    /// Get Cache info
    std::string GetInfo();


    /// Construct a table
    /// @param name of the table
    /// @param type if dimension or fact table
    /// @param uri is a uniform resource allocator for raw file
    Status ReadTable(std::string name, TableType type, std::string uri);

    /// Make maps for a given table name
    int MakeMaps(std::string tableName);
    int MakeMaps(std::shared_ptr<TTable> ttable);
    
    /// Make maps for all dimension tables
    int MakeMaps();


    // TBD
    // Define various cuts here - slicing and dicing
    // Examples are - point, range, set

    // Defining first cut here based on index, it creates a new arrow table
    // https://cubes.readthedocs.io/en/v1.0.1/slicing_and_dicing.html

    // This gives a slice from offset from beginning of length length
    std::shared_ptr<arrow::Table> Slice(std::string tableName, int64_t offset, int64_t length);

  private:

    /// A singleton cache keeps all the tables
    static std::shared_ptr<TCache> tCache_;

    /// Get Id for given table name and field name
    /// @param tableName name of the table
    /// returns ptr to TTable, null if not present
    std::shared_ptr<TTable> GetTTable(std::string tableName);
    

  };

};
