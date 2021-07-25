#pragma once

#include <common.h>
#include <TCacheTypes.h>

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
// TBD Add RecordBatchReader and being able to add to the table in streaming fashion
//

namespace liten
{

/// TCache maintains a Liten cache
class TCache
{
public:

  /// Get a singleton instance, if not present create one
  static std::shared_ptr<TCache> GetInstance();
    
  /// Get Cache info
  std::string GetInfo();

  /// Construct a table in cache from csv
  /// @param name of the table
  /// @param type if dimension or fact table
  /// @param csvUri is a uniform resource allocator for raw file
  /// @param readOptions
  /// @param parseOptions
  /// @param convertOptions
  /// @returns Status
  /// @exception Arrow error exceptions for internal errors
  TResult<std::shared_ptr<TTable>> ReadCsv(std::string tableName,
                                           TableType type,
                                           std::string csvUri,
                                           const arrow::csv::ReadOptions& readOptions,
                                           const arrow::csv::ParseOptions& parseOptions,
                                           const arrow::csv::ConvertOptions& convertOptions);


  /// Add table to cache
  /// @param tableName name of table
  /// @param type fact or dimension table
  /// @param table arrow table to be added
  /// @returns Result with TTable
  TResult<std::shared_ptr<TTable>> AddTable(std::string tableName,
                                            TableType type,
                                            std::shared_ptr<arrow::Table> table);

  /// Get table or given table name
  /// @param tableName name of the table
  /// @returns ptr to TTable, null if not present
  std::shared_ptr<TTable> GetTable(std::string tableName) const;

  /// join schema field to to another schema field
  //  $$$$$
  
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

  /// Default destructor
  ~TCache() { }
    
private:

  /// A singleton cache keeps all the tables
  static std::shared_ptr<TCache> tCache_;

  /// Cannot be constructed
  TCache() { }

  /// Allow shared_ptr with private constructors
  struct MakeSharedEnabler;
    
};

struct TCache::MakeSharedEnabler : public TCache
{
  MakeSharedEnabler() : TCache() { }
};  

}
