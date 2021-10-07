#include <TBlock.h>
#include <TCache.h>
#include <TCatalog.h>
#include <TTable.h>
#include <TSchema.h>

namespace liten
{

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
    tCache_ = std::make_shared<TCache::MakeSharedEnabler>();
  }
  TLOG(INFO) << "Created a new TCache";
  return tCache_;
}

TResult<std::shared_ptr<TTable>> TCache::AddTable(std::string tableName,
                                                  TableType type,
                                                  std::string schemaName)
{
  return std::move(TTable::Create(tableName, type, schemaName));
}

TResult<std::shared_ptr<TRowBlock>> TCache::AddRowBlock(std::shared_ptr<TTable> ttable,
                                                std::shared_ptr<arrow::RecordBatch> recordBatch)
{
  return std::move(ttable->AddRowBlock(recordBatch));
}

TStatus TCache::AddArrowTable(std::shared_ptr<TTable> ttable,
                              std::shared_ptr<arrow::Table> table)
{
  return std::move(ttable->AddArrowTable(table));
}

TResult<std::shared_ptr<TSchema>> TCache::AddSchema(std::string schemaName,
                                                    TableType type,
                                                    std::shared_ptr<arrow::Schema> schema)
{
  return std::move(TSchema::Create(schemaName, type, schema));
}

std::shared_ptr<TTable> TCache::GetTable(std::string tableName) const
{
  return (TCatalog::GetInstance()->GetTable(tableName));
}

std::shared_ptr<TSchema> TCache::GetSchema(std::string schemaName) const
{
  return (TCatalog::GetInstance()->GetSchema(schemaName));
}

// Get Cache information
std::string TCache::GetInfo()
{
  std::stringstream ss;
  ss << "{\"Compute\":";
  ss << TConfigs::GetInstance()->GetComputeInfo();
  ss << ",\"Table\":";
  ss << TCatalog::GetInstance()->GetTableInfo();
  ss << ",\"Schema\":";
  ss << TCatalog::GetInstance()->GetSchemaInfo();
  ss << "}";
  return std::move(ss.str());
}

std::string TCache::GetComputeInfo()
{
  std::stringstream ss;
  ss << "{\"Compute\":";
  ss << TConfigs::GetInstance()->GetComputeInfo();
  ss << "}";
  return std::move(ss.str());
}

std::string TCache::GetTableInfo()
{
  std::stringstream ss;
  ss << "{\"Table\":";
  ss << TCatalog::GetInstance()->GetTableInfo();
  ss << "}";
  return std::move(ss.str());
}

std::string TCache::GetSchemaInfo()
{
  std::stringstream ss;
  ss << "{\"Schema\":";
  ss << TCatalog::GetInstance()->GetSchemaInfo();
  ss << "}";
  return std::move(ss.str());
}


/// Read csv file in a new table tableName. tableName should be unique
TResult<std::shared_ptr<TTable>> TCache::ReadCsvTable(std::string tableName,
                                                      TableType type,
                                                      std::string csvUri,
                                                      const arrow::csv::ReadOptions& readOptions,
                                                      const arrow::csv::ParseOptions& parseOptions,
                                                      const arrow::csv::ConvertOptions& convertOptions)
{
  // If found one, return it
  std::shared_ptr<TTable> ttable = TCatalog::GetInstance()->GetTable(tableName);
  if (nullptr != ttable)
  {
    TLOG(INFO) << csvUri << " found in cache memory for tableName=" << tableName;
    return TResult<std::shared_ptr<TTable>>(ttable);
  }

  // Create one table with tableName
  // Use default memory pool
  arrow::MemoryPool* pool = arrow::default_memory_pool();

  // Readable File for the csvFile
  arrow::Result<std::shared_ptr<arrow::io::ReadableFile>> fpResult =
    arrow::io::ReadableFile::Open(csvUri, pool);
  if (!fpResult.ok()) {
    TLOG(ERROR) << "Cannot open file " << csvUri;
    return TStatus::Invalid("Cannot open file=", csvUri);
  }
  std::shared_ptr<arrow::io::ReadableFile> fp = fpResult.ValueOrDie();

  // Get fileSizeResult
  arrow::Result<int64_t> fileSizeResult = fp->GetSize();
  if (!fileSizeResult.ok()) {
    TLOG(ERROR) << "Unknown filesize for file " << csvUri;
    return TResult<std::shared_ptr<TTable>>(TStatus::UnknownError("Unknown filesize for file ", csvUri));
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
    TLOG(ERROR) << "Cannot read table " << csvUri;
    return TStatus::IOError("Cannot read table=", csvUri);
  }
  std::shared_ptr<arrow::csv::TableReader> reader = readerResult.ValueOrDie();

  // Read table from CSV file
  arrow::Result<std::shared_ptr<arrow::Table>> tableResult = reader->Read();
  if (!tableResult.ok()) {
    // Handle CSV read error
    // (for example a CSV syntax error or failed type conversion)
    TLOG(ERROR) << "Reading csv table= " << tableResult.status().ToString();
    return TResult<std::shared_ptr<TTable>>(TStatus::IOError("Reading csv table= ", tableResult.status().ToString()));
  }
  std::shared_ptr<arrow::Table> table = tableResult.ValueOrDie();

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
  if (nullptr == table)
  {
    return TResult<std::shared_ptr<TTable>>(TStatus::UnknownError("Creating arrow table"));
  }
  auto ttableResult = std::move(TTable::Create(tableName, type, ""));
  if (!ttableResult.ok())
  {
    TLOG(ERROR) << "Error creating Liten table= " << tableName;
  }
  ttable = ttableResult.ValueOrDie();
  auto status = ttable->AddArrowTable(table);
  if (!status.ok())
    return status;
  return ttableResult;
}

/// Read csv file in a new table tableName. tableName should be unique
TResult<std::shared_ptr<TTable>> TCache::ReadCsv(std::string tableName,
                                                 TableType type,
                                                 std::string csvUri,
                                                 const arrow::csv::ReadOptions& readOptions,
                                                 const arrow::csv::ParseOptions& parseOptions,
                                                 const arrow::csv::ConvertOptions& convertOptions)
{
  // If found one, return it
  std::shared_ptr<TTable> ttable = TCatalog::GetInstance()->GetTable(tableName);
  if (nullptr != ttable)
  {
    TLOG(INFO) << csvUri << " found in cache memory for tableName=" << tableName;
    return TResult<std::shared_ptr<TTable>>(ttable);
  }

  // Create one table with tableName
  // Use default memory pool
  arrow::MemoryPool* pool = arrow::default_memory_pool();

  // Readable File for the csvFile
  arrow::Result<std::shared_ptr<arrow::io::ReadableFile>> fpResult =
    arrow::io::ReadableFile::Open(csvUri, pool);
  if (!fpResult.ok()) {
    TLOG(ERROR) << "Cannot open file " << csvUri;
    return TStatus::Invalid("Cannot open file=", csvUri);
  }
  std::shared_ptr<arrow::io::ReadableFile> fp = fpResult.ValueOrDie();

  // Get fileSizeResult
  arrow::Result<int64_t> fileSizeResult = fp->GetSize();
  if (!fileSizeResult.ok()) {
    TLOG(ERROR) << "Unknown filesize for file " << csvUri;
    return TResult<std::shared_ptr<TTable>>(TStatus::UnknownError("Unknown filesize for file ", csvUri));
  }
  int64_t fileSize = fileSizeResult.ValueOrDie();

  // Random access file reader
  std::shared_ptr<arrow::io::InputStream> inputStream =
    arrow::io::RandomAccessFile::GetStream(fp, 0, fileSize);

  // Instantiate TableReader from input stream and options
  arrow::io::IOContext ioContext = arrow::io::default_io_context();
  arrow::Result<std::shared_ptr<arrow::csv::StreamingReader>> readerResult
    = arrow::csv::StreamingReader::Make(ioContext, inputStream, readOptions,
                                    parseOptions, convertOptions);
  if (!readerResult.ok()) {
    TLOG(ERROR) << "Cannot read table " << csvUri;
    return TStatus::IOError("Cannot read table=", csvUri);
  }
  std::shared_ptr<arrow::csv::StreamingReader> reader = readerResult.ValueOrDie();

  // Schema is read from first batch, so should be same in all batches
  std::shared_ptr<arrow::RecordBatch> rb;
  auto rbResult = reader->ReadNext(&rb);
  if (!rbResult.ok()) {
    TLOG(ERROR) << "Reading csv table= " << rbResult.ToString();
    return TResult<std::shared_ptr<TTable>>(TStatus::IOError("Reading csv table= ", rbResult.ToString()));
  }
  
  auto ttableResult = std::move(AddTable(tableName, type, ""));
  if (!ttableResult.ok()) {
    // Handle CSV read error
    // (for example a CSV syntax error or failed type conversion)
    TLOG(ERROR) << "Creating TTable from csv= " << ttableResult.status().ToString();
    return TResult<std::shared_ptr<TTable>>(TStatus::IOError("Reading csv table= ", ttableResult.status().ToString()));
  }
  ttable = ttableResult.ValueOrDie();

  while (rb) {
    auto trbResult = std::move(AddRowBlock(ttable, rb));
    if (!trbResult.ok()) {
      // Handle CSV read error
      // (for example a CSV syntax error or failed type conversion)
      TLOG(ERROR) << "Creating RowBlock from csv= " << trbResult.status().ToString();
      return TResult<std::shared_ptr<TTable>>(TStatus::IOError("Reading csv table= ", trbResult.status().ToString()));
    }
    reader->ReadNext(&rb);
  }

  // Log table information
  auto numCols = ttable->NumColumns();
  TLOG(INFO) << "Total columns=" << numCols;
  return std::move(ttableResult);
}

// TBD modify these
TStatus TCache::MakeMaps(std::string tableName, bool ifReverseMap)
{
  auto ttable = TCatalog::GetInstance()->GetTable(tableName);
  auto result = MakeMaps(ttable,ifReverseMap);
  return result;
}

TStatus TCache::MakeMaps(std::shared_ptr<TTable> ttable, bool ifReverseMap)
{
  if (nullptr == ttable)
  {
    return TStatus::Invalid("Failed to create data-tensor. Did not find in cache table ", ttable->GetName());
  }
  auto result = std::move(ttable->MakeMaps(ifReverseMap));
  return result;
}

TStatus TCache::MakeMaps(bool ifReverseMap)
{
  auto& tables = TCatalog::GetInstance()->GetTableMap();
  for (auto it=tables.begin(); it != tables.end(); it++)
  {
    auto table = it->second;
    auto status = std::move(MakeMaps(table, ifReverseMap));
    if (!status.ok())
    {
      return status;
    }
  }
  return TStatus::OK();
}

TStatus TCache::MakeTensor(std::string tableName)
{
  auto ttable = TCatalog::GetInstance()->GetTable(tableName);
  auto result = std::move(MakeTensor(ttable));
  return result;
}

TStatus TCache::MakeTensor(std::shared_ptr<TTable> ttable)
{
  if (nullptr == ttable)
  {
    return TStatus::Invalid("Failed to create data-tensor. Did not find in cache table ", ttable->GetName());
  }
  auto result = std::move(ttable->MakeTensor());
  return result;
}

TStatus TCache::MakeTensor()
{
  auto& tables = TCatalog::GetInstance()->GetTableMap();
  for (auto it=tables.begin(); it != tables.end(); it++)
  {
    auto table = it->second;
    auto status = std::move(table->MakeTensor());
    if (!status.ok())
    {
      return status;
    }
  }
  return TStatus::OK();
}

// This gives a slice from offset from beginning of length length
// TBD do it using tensor
std::shared_ptr<arrow::Table> TCache::Slice(std::string tableName, int64_t offset, int64_t length)
{
  auto ttable = GetTable(tableName);
  // No table by this name
  if (nullptr == ttable) {
    return nullptr;
  }

  auto slicedTable = ttable->Slice(offset, length);
  return slicedTable;
}
}
