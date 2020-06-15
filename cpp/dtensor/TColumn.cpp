#include "dtensor.h"
#include <iostream>

namespace tendb {

  std::shared_ptr<TColumnChunk> TColumn::GetColumnChunk(int64_t rowNum)
  {
    assert (rowNum < totalComponents_);
    for (int32_t i=1; i<totalChunks_; i++)
    {
      if (rowNum < startOffsets_[i])
        return columnChunks_[i];
    }
    return columnChunks_[totalChunks_-1];
  };
}
