#include <iostream>
#include "TTable.h"
#include <arrow/csv/api.h>
#include <arrow/filesystem/api.h>
#include <arrow/io/api.h>


namespace tendb {

  void TTable::Print()
  {
    // Print Table for now
    const std::vector<std::shared_ptr<arrow::Field>>& tableSchemaFields = schema_->fields();
    std::cout << "Schema=";

    for (auto schemaField : tableSchemaFields) 
    {
      std::cout << "{" << schemaField->ToString() << "}," ;
    }
    std::cout << std::endl;
  
    std::cout << "NumCols=" << NumColumns() << std::endl;
    std::cout << "NumRows=" << NumRows() << std::endl;

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
            std::cout << ",";
          }
          else
          {
            std::shared_ptr<arrow::Scalar> data = dataResult.ValueOrDie();
            // todo
            //auto typeData = static_cast<decltype(colFieldType.get())>(data.get());
            //std::cout << typeData?typeData->ToString():"" << ",";
            if (data->is_valid)
              std::cout << data->ToString();
            std::cout << ",";
          }
        }
        std::cout << std::endl;
      }
    }
  }

  // TODO status & log
  bool TTable::Read(std::string csvFileName)
  {
    // A default memory pool
    arrow::MemoryPool* pool = arrow::default_memory_pool();

    // Readable File
    arrow::Result<std::shared_ptr<arrow::io::ReadableFile>> fpResult = arrow::io::ReadableFile::Open(csvFileName, pool);
    if (!fpResult.ok()) {
      std::cout << "Cannot open file " << csvFileName << std::endl;
      return false;
    }
    std::shared_ptr<arrow::io::ReadableFile> fp = fpResult.ValueOrDie();
  
    arrow::Result<int64_t> fileSizeResult = fp->GetSize();
    if (!fileSizeResult.ok()) {
      std::cout << "Unknown filesize for file " << csvFileName << std::endl;
      return false;
    }
    int64_t fileSize = fileSizeResult.ValueOrDie();
  
    std::shared_ptr<arrow::io::InputStream> inputStream = arrow::io::RandomAccessFile::GetStream(fp, 0, fileSize);

    auto read_options = arrow::csv::ReadOptions::Defaults();
    auto parse_options = arrow::csv::ParseOptions::Defaults();
    auto convert_options = arrow::csv::ConvertOptions::Defaults();

    // Instantiate TableReader from input stream and options
    arrow::Result<std::shared_ptr<arrow::csv::TableReader>> readerResult
      = arrow::csv::TableReader::Make(pool, inputStream, read_options,
                                    parse_options, convert_options);
    if (!readerResult.ok()) {
      std::cout << "Cannot read table " << csvFileName << std::endl;
      return EXIT_SUCCESS;
    }
    std::shared_ptr<arrow::csv::TableReader> reader = readerResult.ValueOrDie();
  
    // Read table from CSV file
    arrow::Result<std::shared_ptr<arrow::Table>> tableResult = reader->Read();
    if (!tableResult.ok()) {
      // Handle CSV read error
      // (for example a CSV syntax error or failed type conversion)
      std::cout << "Error: reading table" << std::endl;
      return EXIT_SUCCESS;
    }
    table_ = tableResult.ValueOrDie();
    schema_ = table_->schema();
    return true;
  }

}
