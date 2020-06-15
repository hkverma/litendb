#include "dtensor.h"
#include <iostream>

namespace tendb {

  void TColumnChunk::Print()
  {
    std::cout << "Seq="  << sequenceNum_;
    std::cout << " Col="  << columnNum_;
    std::cout << " Offset=" << startOffset_;
    std::cout << " Components=" << totalComponents_;
    std::cout << std::endl;

    std::shared_ptr<arrow::Array> component = TCache::GetArray(uuid_);

    for (int64_t i=0; i<component->length(); i++)
    {
      std::shared_ptr<arrow::Scalar> compVal = TCache::GetScalar(component, i);
      //TODO std::cout << compVal->ToString() << "," ;
    }     
    std::cout << std::endl;
 }
    
}
