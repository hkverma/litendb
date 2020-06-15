#include "dtensor.h"
#include <iostream>

namespace tendb {

    // For now the columnChunk should be present here. 
    // In future it could not be here, in that case fetch it if not present here
  std::unordered_map<boost::uuids::uuid, std::shared_ptr<arrow::Array>> TCache::arrays_;

  std::shared_ptr<arrow::Array> TCache::GetArray(boost::uuids::uuid id)
  {
    auto itr = arrays_.find(id);
    assert(itr !=  arrays_.end());
    return itr->second;
  }
  
  std::shared_ptr<arrow::Scalar> TCache::GetScalar(std::shared_ptr<arrow::Array> comp, int64_t rowNum)
  {
    arrow::Result<std::shared_ptr<arrow::Scalar>> compValResult = comp->GetScalar(rowNum);
    assert(compValResult.ok());
    std::shared_ptr<arrow::Scalar> compVal = compValResult.ValueOrDie();
    return compVal;    
  }

}
