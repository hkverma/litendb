#include "dtensor.h"
#include <iostream>

namespace tendb {

  void TColumnChunk::Print()
  {
    std::cout << " Id="  << uuid_;    
    std::cout << " Seq="  << sequenceNum_;
    std::cout << " Col="  << columnNum_;
    std::cout << " Offset=" << startOffset_;
    std::cout << " Components=" << totalComponents_;
    std::cout << std::endl;

 }
    
}
