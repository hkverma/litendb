#include "dtensor.h"
#include <iostream>

namespace tendb {

  
  TColumn::TColumn(std::shared_ptr<arrow::ChunkedArray> chary) :
    chunkedArray_(chary)
  {
    map_ = TArrayMap::Make(chary);
  }

  std::shared_ptr<TColumn> TColumn::Make(std::shared_ptr<arrow::ChunkedArray> chunkedArray)
  {
    std::shared_ptr<TColumn> tColumn = std::make_shared<TColumn>(chunkedArray);
    return tColumn;
  }

};
