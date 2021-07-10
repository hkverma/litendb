#pragma once

#include <common.h>


//
// provide hash template with uuid
//
namespace std
{
  template<>
  struct hash<boost::uuids::uuid>
  {
    size_t operator () (const boost::uuids::uuid& uid) const
    {
        return boost::hash<boost::uuids::uuid>()(uid);
    }
  };
  
};

namespace liten {
  /// Zero cost wrapper class for Arrow Array
  class TBlock {
  public:
    /// Wrap Arrow array in TBlock
    /// @param arr Arrow array
    TBlock(std::shared_ptr<arrow::Array> arr) : arr_(arr) { }
    
    /// Destruct the array, nothing here for now
    ~TBlock() { }
    
    /// Get Raw Array, Use it judiciously to enable easy transformations
    std::shared_ptr<arrow::Array> GetArray();
  
  private:
    std::shared_ptr<arrow::Array> arr_;
  };

};
