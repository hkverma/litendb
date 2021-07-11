#pragma once

#include <common.h>

namespace liten {
  
  /// Zero cost wrapper class for Arrow Array
  class TSchema {
  public:
    
    /// Wrap Arrow array in TSchema
    /// @param schema Arrow schema
    TSchema(std::shared_ptr<arrow::Schema> schema) : schema_(schema) { }
    
    /// Destruct the array, nothing here for now
    ~TSchema() { }

    /// Get Raw Array, Use it judiciously, prefer to add an access method
    std::shared_ptr<arrow::Schema> GetSchema();
  
  private:

    /// Arrow array
    std::shared_ptr<arrow::Schema> schema_;

  };

  inline std::shared_ptr<arrow::Schema> TSchema::GetSchema()
  {
    return schema_;
  }
  
};
