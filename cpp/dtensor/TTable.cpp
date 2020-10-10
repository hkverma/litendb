#include <iostream>
#include <sstream>
#include <arrow/csv/api.h>
#include <arrow/filesystem/api.h>
#include <arrow/io/api.h>

#include <common.h>
#include "TTable.h"


namespace tendb {

  void TTable::PrintSchema()
  {
    // Print Table for now
    const std::vector<std::shared_ptr<arrow::Field>>& tableSchemaFields = schema_->fields();
    std::stringstream ss;
    ss << "Schema=";

    for (auto schemaField : tableSchemaFields) 
    {
      ss << "{" << schemaField->ToString() << "}," ;
    }
    LOG(INFO) << ss.str();
  }

  void TTable::PrintTable()
  {
    std::stringstream ss;
    ss << "NumCols=" << NumColumns();
    ss << " NumRows=" << NumRows() << " Data=";

    // Print the table
    for (int64_t i=0; i<NumColumns(); i++)
    {
      auto chunkedArray = table_->column(i);
      //const std::shared_ptr<arrow::Field>& colField = schema_->field(i);
      //const std::shared_ptr<arrow::DataType>& colFieldType = colField->type();
       
      for (int64_t j=0; j<chunkedArray->num_chunks(); j++)
      {
        auto aray = chunkedArray->chunk(j);
        for (int64_t k=0; k<aray->length(); k++)
        {
          arrow::Result<std::shared_ptr<arrow::Scalar>> dataResult = aray->GetScalar(k);
          if ( !dataResult.ok() )
          {
            ss << ",";
          }
          else
          {
            std::shared_ptr<arrow::Scalar> data = dataResult.ValueOrDie();
            // todo
            //auto typeData = static_cast<decltype(colFieldType.get())>(data.get());
            //ss << typeData?typeData->ToString():"" << ",";
            if (data->is_valid)
              ss << data->ToString();
            ss << ",";
          }
        }
      }
    }
    LOG(INFO) << ss.str();
  }

  // status & log
  bool TTable::ReadCsv(std::string csvFileName,
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
      return false;
    }
    std::shared_ptr<arrow::io::ReadableFile> fp = fpResult.ValueOrDie();

    // Get fileSizeResult
    arrow::Result<int64_t> fileSizeResult = fp->GetSize();
    if (!fileSizeResult.ok()) {
      LOG(ERROR) << "Unknown filesize for file " << csvFileName;
      return false;
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
      return false;
    }
    std::shared_ptr<arrow::csv::TableReader> reader = readerResult.ValueOrDie();
  
    // Read table from CSV file
    arrow::Result<std::shared_ptr<arrow::Table>> tableResult = reader->Read();
    if (!tableResult.ok()) {
      // Handle CSV read error
      // (for example a CSV syntax error or failed type conversion)
      LOG(ERROR) << "Reading csv table";
      return false;
    }
    
    table_ = tableResult.ValueOrDie();
    schema_ = table_->schema();
    return true;
  }

  bool TTable::MakeMaps()
  {
    if (nullptr == table_)
    {
      return false;
    }
    
    for (int64_t cnum=0; cnum<table_->num_columns(); cnum++)
    {
      std::shared_ptr<arrow::ChunkedArray> chArr = table_->column(cnum);
      auto colMap = TColumnMap::Make(chArr);
      maps_.push_back(colMap);
    }
    return true;
  }

  void TTable::PrintMaps()
  {
    std::stringstream ss;
    for (int colNum = 0; colNum < maps_.size(); colNum++) {
      auto colMap = maps_[colNum];
      ss << "Col " << colNum;
      for (int arrNum = 0; arrNum< colMap->arrayMap_.size(); arrNum++)
      {
        auto arrMap = colMap->arrayMap_[arrNum];
        int64_t minVal, maxVal;
        ss << " Arr " << arrNum << " Size=" << arrMap->array_->length();
        ss << " Type=" << arrMap->array_->type()->ToString() ;
        ss << " Min=";
        arrMap->GetMin(minVal)?(ss << minVal):(ss << "None");
        ss << " Max=";
        arrMap->GetMax(maxVal)?(ss << maxVal):(ss << "None");
        ss << ";" ;
        // arrMap->GetReverseMap(ss);
        // ss << "; ";
      }
    }
    LOG(INFO) << ss.str();
  }
}
