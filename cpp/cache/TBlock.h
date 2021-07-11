#pragma once

#include <common.h>

namespace liten {
  /// Zero cost wrapper class for Arrow Array
  class TBlock {
  public:
    /// Wrap Arrow array in TBlock
    /// @param arr Arrow array
    TBlock(std::shared_ptr<arrow::Array> arr) : arr_(arr) { }
    
    /// Destruct the array, nothing here for now
    ~TBlock() { }

    /// Get Raw Array, Use it judiciously, prefer if you add an access method
    std::shared_ptr<arrow::Array> GetArray();
  
  private:

    /// Arrow array
    std::shared_ptr<arrow::Array> arr_;

  };

  
};
