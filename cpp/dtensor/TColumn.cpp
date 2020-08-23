#include "dtensor.h"
#include <iostream>

namespace tendb {

  
  TColumn::TColumn(std::shared_ptr<arrow::ChunkedArray> chary) :
    chunkedArray_(chary)
  {
  }

  std::shared_ptr<TColumn> TColumn::Make(std::shared_ptr<arrow::ChunkedArray> chunkedArray)
  {
    arrow::Type::type type = chunkedArray->type()->id();
    std::shared_ptr<TColumn> tColumn;
    switch (type)
    {
    case arrow::Type::NA: case arrow::Type::BOOL:
      {
        tColumn = std::make_shared<TColumn>(chunkedArray);
        break ;
      }
    case arrow::Type::UINT8: case arrow::Type::INT8: case arrow::Type::UINT16:
    case arrow::Type::INT16: case arrow::Type::UINT32: case arrow::Type::INT32:
    case arrow::Type::UINT64: case arrow::Type::INT64:
      {
        tColumn = std::make_shared<TIntegralColumn<int64_t>>(chunkedArray);
        break ;
      }
    case arrow::Type::HALF_FLOAT: case arrow::Type::FLOAT: case arrow::Type::DOUBLE:
      {
        tColumn = std::make_shared<TFloatColumn<float>>(chunkedArray);
        break ;
      }        
    case arrow::Type::STRING: case arrow::Type::BINARY: case arrow::Type::FIXED_SIZE_BINARY:
    case arrow::Type::LARGE_STRING: case arrow::Type::LARGE_BINARY:
      {
        tColumn = std::make_shared<TColumn>(chunkedArray);
        break ;
      }
    case arrow::Type::DATE32: case arrow::Type::DATE64: case arrow::Type::TIMESTAMP:
    case arrow::Type::TIME32: case arrow::Type::TIME64: case arrow::Type::INTERVAL_MONTHS:
    case arrow::Type::INTERVAL_DAY_TIME: case arrow::Type::DURATION:
      {
        tColumn = std::make_shared<TDateColumn<int64_t>>(chunkedArray);
        break ;
      }
    case arrow::Type::DECIMAL:
      {
        tColumn = std::make_shared<TColumn>(chunkedArray);
        break ;
      }
    case arrow::Type::LIST: case arrow::Type::STRUCT: case arrow::Type::UNION:
    case arrow::Type::DICTIONARY: case arrow::Type::MAP: case arrow::Type::FIXED_SIZE_LIST:
    case arrow::Type::LARGE_LIST:
      {
        tColumn = std::make_shared<TColumn>(chunkedArray);
        break ;
      }

    default:
      {
        tColumn = std::make_shared<TColumn>(chunkedArray);
        break ;
      }
    }
    return tColumn;
  }

};
