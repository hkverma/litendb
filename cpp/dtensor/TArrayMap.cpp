#include "dtensor.h"
#include <iostream>

namespace tendb {

  // TODO
  // Need builder for each type
  // go through the list and collect min & max
  // Use buffer and append routines from arrow builders
  //
  // arrow::builder.cc check L22 for the types shown below
  //
  std::shared_ptr<TArrayMap> TArrayMap::Make(std::shared_ptr<arrow::ChunkedArray> chunkedArray)
  {
    std::shared_ptr<TArrayMap> arrayMap_;
    switch (chunkedArray->type()->id())
    {
      /* TODO 
         case UInt8: case Int8: case UInt16:  case Int16:
         case UInt32:
         case Int32:
         case UInt64:
      */
    case arrow::Int64Type::type_id:
    {
      arrayMap_ = std::make_shared<TInt64ArrayMap>(chunkedArray);
    }
    /* TODO
       case Date32:
       case Date64:
       case Duration:
       case Time32:
       case Time64:
       case Timestamp:
       case MonthInterval:
       case DayTimeInterval:
       case Boolean:
       case HalfFloat:
       case Float:
       case Double:
       case String:
       case Binary:
       case LargeString:
       case LargeBinary:
       case FixedSizeBinary:
       case Decimal128:
    */
    default:
      {
        break ;
      }
    }
    return arrayMap_;
  }
      
    
  // Create an map based on the type of data type & map type
  /*
    std::shared_ptr<TArrayMap> TArrayMap::Make(std::shared_ptr<arrow::ChunkedArray> chunkedArray,
    MapType mapType)
    {
    
    std::shared_ptr<TArrayMap> tArrayMap;

    switch(mapType)
    {
    case None:
    {
    tArrayMap = std::make_shared<TArrayMap>(chunkedArray);
    break;
    }
    case Range:
    {
    tArrayMap = MakeRange(chunkedArray);
    break;
    }
    default:
    {
    tArrayMap = std::make_shared<TArrayMap>(chunkedArray);
    break;      
    }

    }
    return tArrayMap;
    }
  
    std::shared_ptr<TArrayMap> TArrayMap::MakeRange(std::shared_ptr<arrow::ChunkedArray> chunkedArray)
    {
    std::shared_ptr<TArrayMap> tArrayMap;      
    arrow::Type::type type = chunkedArray->type()->id();
    
    switch (type)
    {
    case arrow::Type::NA: case arrow::Type::BOOL:
    {
    tArrayMap = std::make_shared<TArrayMap>(chunkedArray);
    break ;
    }
    case arrow::Type::UINT8: case arrow::Type::INT8: case arrow::Type::UINT16:
    case arrow::Type::INT16: case arrow::Type::UINT32: case arrow::Type::INT32:
    case arrow::Type::UINT64: case arrow::Type::INT64:
    {
    tArrayMap = std::make_shared<TComparablesMap<int64_t>>(chunkedArray);
    break ;
    }
    case arrow::Type::HALF_FLOAT: case arrow::Type::FLOAT: case arrow::Type::DOUBLE:
    {
    tArrayMap = std::make_shared<TFloatArrayMap<float>>(chunkedArray);
    break ;
    }        
    case arrow::Type::STRING: case arrow::Type::BINARY: case arrow::Type::FIXED_SIZE_BINARY:
    case arrow::Type::LARGE_STRING: case arrow::Type::LARGE_BINARY:
    {
    tArrayMap = std::make_shared<TArrayMap>(chunkedArray);
    break ;
    }
    case arrow::Type::DATE32: case arrow::Type::DATE64: case arrow::Type::TIMESTAMP:
    case arrow::Type::TIME32: case arrow::Type::TIME64: case arrow::Type::INTERVAL_MONTHS:
    case arrow::Type::INTERVAL_DAY_TIME: case arrow::Type::DURATION:
    {
    tArrayMap = std::make_shared<TDateArrayMap<int64_t>>(chunkedArray);
    break ;
    }
    case arrow::Type::DECIMAL:
    {
    tArrayMap = std::make_shared<TArrayMap>(chunkedArray);
    break ;
    }
    case arrow::Type::LIST: case arrow::Type::STRUCT: case arrow::Type::UNION:
    case arrow::Type::DICTIONARY: case arrow::Type::MAP: case arrow::Type::FIXED_SIZE_LIST:
    case arrow::Type::LARGE_LIST:
    {
    tArrayMap = std::make_shared<TArrayMap>(chunkedArray);
    break ;
    }

    default:
    {
    tArrayMap = std::make_shared<TArrayMap>(chunkedArray);
    break ;
    }
    }
    return tColumn;
    
    }
    // Make a RangeMap
    static std::shared_ptr<TArrayMap> TArrayMap::Make(std::shared_ptr<arrow::ChunkedArray> chunkedArray,
    MapType mapType)
    {
    std::shared_ptr<TArrayMap> arrayMap;
    switch (type)
    {
    case arrow::Type::BOOL:
    {
    arrayMap = std::make_shared<TRangeMap<int8_t>(chunkedArray);
    break ;
    }
    case arrow::Type::UINT8:
    {
    arrayMap = std::make_shared<TRangeMap<uint8_t>(chunkedArray);
    break ;
    }
    case arrow::Type::INT8:
    {
    arrayMap = std::make_shared<TRangeMap<int8_t>(chunkedArray);
    break ;
    }
    case arrow::Type::UINT16:
    {
    arrayMap = std::make_shared<TRangeMap<uint16_t>(chunkedArray);
    break ;
    }
    case arrow::Type::INT16:
    {
    }
    case arrow::Type::UINT32: case arrow::Type::INT32:
    case arrow::Type::UINT64: case arrow::Type::INT64:
    {
    tArrayMap = std::make_shared<TComparablesMap<int64_t>>(chunkedArray);
    break ;
    }
    case arrow::Type::HALF_FLOAT: case arrow::Type::FLOAT: case arrow::Type::DOUBLE:
    {
    tArrayMap = std::make_shared<TFloatArrayMap<float>>(chunkedArray);
    break ;
    }        
    case arrow::Type::STRING: case arrow::Type::BINARY: case arrow::Type::FIXED_SIZE_BINARY:
    case arrow::Type::LARGE_STRING: case arrow::Type::LARGE_BINARY:
    {
    tArrayMap = std::make_shared<TArrayMap>(chunkedArray);
    break ;
    }
    case arrow::Type::DATE32: case arrow::Type::DATE64: case arrow::Type::TIMESTAMP:
    case arrow::Type::TIME32: case arrow::Type::TIME64: case arrow::Type::INTERVAL_MONTHS:
    case arrow::Type::INTERVAL_DAY_TIME: case arrow::Type::DURATION:
    {
    tArrayMap = std::make_shared<TDateArrayMap<int64_t>>(chunkedArray);
    break ;
    }
    case arrow::Type::DECIMAL:
    {
    tArrayMap = std::make_shared<TArrayMap>(chunkedArray);
    break ;
    }
    case arrow::Type::LIST: case arrow::Type::STRUCT: case arrow::Type::UNION:
    case arrow::Type::DICTIONARY: case arrow::Type::MAP: case arrow::Type::FIXED_SIZE_LIST:
    case arrow::Type::LARGE_LIST:
    {
    tArrayMap = std::make_shared<TArrayMap>(chunkedArray);
    break ;
    }

    default:
    {
    std::cout << "Unidentified type: " << type_id << std::endl;
    rangeMap = std::make_shared<TRangeMap<int64_t>(chunkedArray);      
    break ;
    }
    }
    return rangeMap;
    }

  */
    
}
