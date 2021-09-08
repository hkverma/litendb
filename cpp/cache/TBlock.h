#pragma once

#include <common.h>

namespace liten {
  
/// Zero cost wrapper class for Arrow Array
class TBlock {
public:

  /// Wrap Arrow array in TBlock
  /// Create a TBlock, if one already exists with the given arr, return that
  /// @param arr arrow::Array for which we need to create TBlock
  static TResult<std::shared_ptr<TBlock>> Create(std::shared_ptr<arrow::Array> arr);
    
  /// Destruct the array, nothing here for now
  ~TBlock() { }

  /// Get Raw Array, Use it judiciously, prefer to add an access method
  std::shared_ptr<arrow::Array> GetArray();

  /// Get TBlock for a given array, return nullptr if not found
  static std::shared_ptr<TBlock> GetTBlock(std::shared_ptr<arrow::Array> arr);
    
  /// Add TBlock to the lookup list
  static TStatus AddTBlock(std::shared_ptr<TBlock> blk);
    
private:

  /// Use named constructor instead
  TBlock() { }
    
  /// Arrow array
  std::shared_ptr<arrow::Array> arr_;

  /// Each block has uuid for persistence and caching
  TGuid::Uuid id_;
    
  /// Arrow Array to TBlock
  // TBD create an atomic template class with mutex in there
  static std::unordered_map<std::shared_ptr<arrow::Array>, std::shared_ptr<TBlock>> arrayToBlock_;
  static std::shared_mutex arrayToBlockMutex_;

  /// Allow shared_ptr with private constructors
  struct MakeSharedEnabler;    
    
};

struct TBlock::MakeSharedEnabler : public TBlock {
  MakeSharedEnabler() : TBlock() { }
};

inline std::shared_ptr<arrow::Array> TBlock::GetArray()
{
  return arr_;
}
  
}
