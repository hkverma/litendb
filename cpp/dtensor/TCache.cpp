//#include <iostream>
#include <common.h>

#include <TBlock.h>
#include <TCache.h>

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
  if (tCache_ == nullptr)
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
  for (auto& tableId : tablesUri_)
  {
    std::string& tableName = tableId.first;
    auto tTable = tableId.second;
    ss << ",\n";
    if (TTable::Dimension == tTable->GetType()) {
      ss << "\"Dim\":\"" << tableName << "\"";
    } else if (TTable::Fact == ttable->GetType()) {
      ss << "\"Fact\":" << tableName << "\"";
    } else {
      ss << "\"Unknown\":" << tableName << "\"";
    }
    //ttable->PrintSchema();
    //ttable->PrintTable();
  }
  ss << "\n}";
  TLog::GetInstance()->FlushLogFile(TLog::Info);
  return ss.str();
}

std::shared_ptr<TTable> TCache::GetTTable(std::string tableName)
{
  auto itr = tables_.find(tableName);
  if (tables_.end() == itr)
    return nullptr;
  return (itr->second);
}


// $$$$$$

// Todo make it thread-safe
std::shared_ptr<TTable> TCache::AddTable(std::string tableName,
                                         TTable::TType type,
                                         std::shared_ptr<arrow::Table> table)
                                         
{
  auto tTable = GetTTable(tableName);
  if (tTable) {
    TLOG(ERROR) << "Adding another table with an existing table name " << tableName;
    return nullptr;
  }
  
  boost::uuids::uuid cacheId;
  if (GetId(tableName, cacheId))
  {
  }
  
  TLOG(INFO) << "Adding new table " << tableName;
  try {
    auto ttable = make_shared<TTable>(tableName, type, table);
    if (nullptr = ttable) {
      TLOG(ERROR) << "Adding table table name " << tableName;
      return nullptr;
    }
    
  }
  catch (std::exception& e)
  {
    TLOG(ERROR) << "Adding table name " << tableName << " failed with exception " << e.what();
  }
  return nullptr;
}

bool TCache::GetId(std::string tableName, boost::uuids::uuid& cacheId)
{
  auto itr = blockIds_.find(tableName);
  if (itr ==  cacheIds_.end())
    return false;
  cacheId = itr->second;
  return true;
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
    TLOG(INFO) << "Cannnot read " << csvFileName << " table name already exists " << tableName;
    return table;
  }

  // Create one table with tableName
  auto arrowTable = ReadCsv(csvFileName, readOptions, parseOptions, convertOptions);
  if (nullptr == arrowTable)
  {
    return nullptr;
  }
  auto ttable = AddTable(tableName, arrowTable, TTable::Dim);
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


int TCache::MakeMaps(std::string tableName)
{
  auto ttable = GetTable(tableName);
  int result = MakeMaps(ttable);
  return result;
}

int TCache::MakeMaps(std::shared_ptr<TTable> ttable)
{
  if (nullptr == ttable)
  {
    TLOG(ERROR) << "Failed to create data-tensor. Did not find in cache table " << ttable->GetName();
    return 1;
  }
  // TODO are numCopies needed? remove it.
  int result = ttable->MakeMaps(1); 
  if (result)
  {
    TLOG(ERROR) << "Found table " << ttable->GetName() << " but failed to create data tensor";
  }
  return result;
}

int TCache::MakeMaps()
{
  int result = 0;
  for (auto it=tables_.begin(); it != tables_.end(); it++)
  {
    auto table = it->second;
    if (MakeMaps(table))
    {
      result = 1;
    }
  }
  return result;
}

// Read CSV file into an arrow table
std::shared_ptr<arrow::Table> TCache::ReadCsv
(std::string csvFileName,
 const arrow::csv::ReadOptions& readOptions,
 const arrow::csv::ParseOptions& parseOptions,
 const arrow::csv::ConvertOptions& convertOptions)
{
  // A default memory pool
  // TODO define liten memory pool
  arrow::MemoryPool* pool = arrow::default_memory_pool();

  // Readable File for the csvFile
  arrow::Result<std::shared_ptr<arrow::io::ReadableFile>> fpResult =
    arrow::io::ReadableFile::Open(csvFileName, pool);
  if (!fpResult.ok()) {
    TLOG(ERROR) << "Cannot open file " << csvFileName;
    return nullptr;
  }
  std::shared_ptr<arrow::io::ReadableFile> fp = fpResult.ValueOrDie();

  // Get fileSizeResult
  arrow::Result<int64_t> fileSizeResult = fp->GetSize();
  if (!fileSizeResult.ok()) {
    TLOG(ERROR) << "Unknown filesize for file " << csvFileName;
    return nullptr;
  }
  int64_t fileSize = fileSizeResult.ValueOrDie();

  // Random access file reader
  std::shared_ptr<arrow::io::InputStream> inputStream =
    arrow::io::RandomAccessFile::GetStream(fp, 0, fileSize);
    
  // Instantiate TableReader from input stream and options
  arrow::io::IOContext ioContext = arrow::io::default_io_context();
  arrow::Result<std::shared_ptr<arrow::csv::TableReader>> readerResult
    = arrow::csv::TableReader::Make(ioContext, inputStream, readOptions,
                                    parseOptions, convertOptions);
  if (!readerResult.ok()) {
    TLOG(ERROR) << "Cannot read table " << csvFileName;
    return nullptr;
  }
  std::shared_ptr<arrow::csv::TableReader> reader = readerResult.ValueOrDie();
  
  // Read table from CSV file
  arrow::Result<std::shared_ptr<arrow::Table>> tableResult = reader->Read();
  if (!tableResult.ok()) {
    // Handle CSV read error
    // (for example a CSV syntax error or failed type conversion)
    TLOG(ERROR) << "Reading csv table";
    return nullptr;
  }
    
  std::shared_ptr<arrow::Table>  table = tableResult.ValueOrDie();
  
  // Log table information
  std::vector<std::shared_ptr<arrow::ChunkedArray>> cols = table->columns();
  TLOG(INFO) << "Total columns=" << cols.size();
  int64_t numChunks = cols[0]->num_chunks();
  for (int i=0; i<cols.size(); i++) {
    if (cols[i]->num_chunks() != numChunks) {
      TLOG(ERROR) << "Chunks " << cols[i]->num_chunks() << " != " << numChunks;
    }
  }
  for (auto i=0; i<cols.size(); i++) {
    for (auto j=0; j<numChunks; j++) {
      if (cols[i]->chunk(j)->length() != cols[0]->chunk(j)->length()) {
        TLOG(ERROR) << "Col " << i << " Chunk " << j ;
        TLOG(ERROR) << "Chunk length " << cols[i]->chunk(j)->length() << "!=" << cols[0]->chunk(j)->length() ;
      }
    }
  }
  return table;
}

// This gives a slice from offset from beginning of length length
// TODO if read in cython, when does shared_ptr go out of scope
std::shared_ptr<arrow::Table> TCache::Slice(std::string tableName, int64_t offset, int64_t length)
{
  auto arrTable = GetTable(tableName);
  // No table by this name
  if (nullptr == arrTable) {
    return nullptr;
  }
  
  auto slicedTable = arrTable->Slice(offset, length);
  return slicedTable;
  
}
