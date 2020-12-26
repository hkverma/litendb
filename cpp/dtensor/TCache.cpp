#include "TCache.h"
#include <iostream>
#include <common.h>

#include <boost/uuid/random_generator.hpp>

using namespace tendb;

// For now the columnChunk should be present here.
// TODO In future it could not be here, in that case fetch it if not present here
//      Use status code to return back instance
//      Use arrow::Result type object for getting value

/// Singleton cache instance
std::shared_ptr<TCache> TCache::tCache_ = nullptr;

/// Get a singleton instance, if not present create one
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
    LOG(INFO) << "Cannnot read " << csvFileName << " table name already exists " << tableName;
    return table;
  }

  // Create one table with tableName
  auto arrowTable = ReadCsv(csvFileName, readOptions, parseOptions, convertOptions);
  if (nullptr == arrowTable)
  {
    return nullptr;
  }
  auto ttable = AddTable(tableName, arrowTable);
  return ttable;
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

bool TCache::GetId(std::string tableName, boost::uuids::uuid& cacheId)
{
  auto itr = cacheIds_.find(tableName);
  if (itr ==  cacheIds_.end())
    return false;
  cacheId = itr->second;
  return true;
}

std::shared_ptr<TTable> TCache::AddTable(std::string tableName,
                                         std::shared_ptr<arrow::Table> table)
{
  boost::uuids::uuid cacheId;
  if (GetId(tableName, cacheId))
  {
    LOG(ERROR) << "Adding another table with an existing table name " << tableName;
    return nullptr;
  }
  
  LOG(INFO) << "Adding new table " << tableName;
  auto ttable = std::make_shared<TTable>(tableName, table);
  cacheId = idGenerator();
  tables_[cacheId] = ttable;
  cacheIds_[tableName] = cacheId;
  return ttable;
}  

// Read CSV file into an arrow table
std::shared_ptr<arrow::Table> TCache::ReadCsv
(std::string csvFileName,
 const arrow::csv::ReadOptions& readOptions,
 const arrow::csv::ParseOptions& parseOptions,
 const arrow::csv::ConvertOptions& convertOptions)
{
  // A default memory pool
  // TODO define tendb memory pool
  arrow::MemoryPool* pool = arrow::default_memory_pool();

  // Readable File for the csvFile
  arrow::Result<std::shared_ptr<arrow::io::ReadableFile>> fpResult =
    arrow::io::ReadableFile::Open(csvFileName, pool);
  if (!fpResult.ok()) {
    LOG(ERROR) << "Cannot open file " << csvFileName;
    return nullptr;
  }
  std::shared_ptr<arrow::io::ReadableFile> fp = fpResult.ValueOrDie();

  // Get fileSizeResult
  arrow::Result<int64_t> fileSizeResult = fp->GetSize();
  if (!fileSizeResult.ok()) {
    LOG(ERROR) << "Unknown filesize for file " << csvFileName;
    return nullptr;
  }
  int64_t fileSize = fileSizeResult.ValueOrDie();

  // Random access file reader
  std::shared_ptr<arrow::io::InputStream> inputStream =
    arrow::io::RandomAccessFile::GetStream(fp, 0, fileSize);
    
  // Instantiate TableReader from input stream and options
  arrow::Result<std::shared_ptr<arrow::csv::TableReader>> readerResult
    = arrow::csv::TableReader::Make(pool, inputStream, readOptions,
                                    parseOptions, convertOptions);
  if (!readerResult.ok()) {
    LOG(ERROR) << "Cannot read table " << csvFileName;
    return nullptr;
  }
  std::shared_ptr<arrow::csv::TableReader> reader = readerResult.ValueOrDie();
  
  // Read table from CSV file
  arrow::Result<std::shared_ptr<arrow::Table>> tableResult = reader->Read();
  if (!tableResult.ok()) {
    // Handle CSV read error
    // (for example a CSV syntax error or failed type conversion)
    LOG(ERROR) << "Reading csv table";
    return nullptr;
  }
    
  std::shared_ptr<arrow::Table>  table = tableResult.ValueOrDie();
  
  // Log table information
  std::vector<std::shared_ptr<arrow::ChunkedArray>> cols = table->columns();
  LOG(INFO) << "Total columns=" << cols.size();
  int64_t numChunks = cols[0]->num_chunks();
  for (int i=0; i<cols.size(); i++) {
    if (cols[i]->num_chunks() != numChunks) {
      LOG(ERROR) << "Chunks " << cols[i]->num_chunks() << " != " << numChunks;
    }
  }
  for (auto i=0; i<cols.size(); i++) {
    for (auto j=0; j<numChunks; j++) {
      if (cols[i]->chunk(j)->length() != cols[0]->chunk(j)->length()) {
        LOG(ERROR) << "Col " << i << " Chunk " << j ;
        LOG(ERROR) << "Chunk length " << cols[i]->chunk(j)->length() << "!=" << cols[0]->chunk(j)->length() ;
      }
    }
  }
  return table;
}
