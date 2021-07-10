#include <TBlock.h>

/// Wrap Arrow array in TBlock
TBlock::TBlock(std::shared_ptr<arrow::Array> arr) : arr_(arr)
{
  
}
