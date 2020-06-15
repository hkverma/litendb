#include "dtensor.h"
#include <iostream>

namespace tendb {

  // Initialize Catalog with this table
  void TTable::Init(std::shared_ptr<arrow::Table> table)
  {
    std::shared_ptr<arrow::Schema> tableSchema = table->schema();
    if (schema_) 
    {
      if (!schema_->Equals(tableSchema))
      {
        std::cout << "Trying multiple schemas for the same table. Init failed" << std::endl;
      }  
    } 
    else 
    { 
      schema_ = tableSchema;
    }
  }
  
  std::shared_ptr<TColumnChunk> TTable::GetColumnChunk(int64_t rowNum, int64_t colNum)
  {
    assert(colNum < numColumns_);
    assert(rowNum < numRows_);
    auto columnChunk = columns_[colNum]->GetColumnChunk(rowNum);
    return columnChunk;
  }
  
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
  
    std::cout << "NumCols=" << numColumns_ << std::endl;
    std::cout << "NumRows=" << numRows_ << std::endl;

    // Print the table
    for (int64_t j=0; j<numRows_; j++)
    {
      for (int64_t i=0; i<numColumns_; i++) {
        auto columnChunk = GetColumnChunk(i, i);
        // TODO std::cout << columnChunk->GetScalar(rowNum)->ToString() << ","
      }
      std::cout << std::endl;
    }
  }
    
}
