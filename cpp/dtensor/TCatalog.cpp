#include "dtensor.h"
#include <iostream>

namespace tendb {

  // Initialize Catalog with this table
  void TCatalog::Init(std::shared_ptr<arrow::Table> table)
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
  
  // Print Table for now
  const std::vector<std::shared_ptr<arrow::Field>>& tableSchemaFields = table->schema()->fields();
  std::cout << "Schema=";

  for (auto schemaField : tableSchemaFields) 
  {
    std::cout << "{" << schemaField->ToString() << "}," ;
    //std::string schemaName = schemaField->name();
    //std::unique_ptr<arrow::ArrayBuilder> *arrayBuilder;
    //arrow::MakeBuilder(pool, schemaField->type(), arrayBuilder);
    //schemaBuilder.push_back(arrayBuilder);
  }
  std::cout << std::endl;
  
  std::cout << "NumCols=" << table->num_columns() << std::endl;
  std::cout << "NumRows=" << table->num_rows() << std::endl;

  //return (TestVectorAndColumnar());
  return;
}
    
}
