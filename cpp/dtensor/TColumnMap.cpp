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
  std::shared_ptr<TArrayMap> TArrayMap::Make(std::shared_ptr<arrow::Array> arr)
  {
    std::shared_ptr<TArrayMap> arrayMap_;
    switch (arr->type()->id())
    {
      /* TODO 
         case UInt8: case Int8: case UInt16:  case Int16:
         case UInt32:
         case Int32:
         case UInt64:
      */
    case arrow::Int64Type::type_id:
    {
      arrayMap_ = TInt64ArrayMap::Make(arr);
      break;
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
      arrayMap_ = std::make_shared<TArrayMap>(arr);
      break ;
    }
    }
    return arrayMap_;
  }

  TInt64ArrayMap::TInt64ArrayMap(std::shared_ptr<arrow::Array> arr) : TArrayMap(arr)
  {
    min_ = std::numeric_limits<int64_t>::min();
    max_ = std::numeric_limits<int64_t>::max();
  }

  std::shared_ptr<TInt64ArrayMap> TInt64ArrayMap::Make(std::shared_ptr<arrow::Array> arr)
  {
    std::shared_ptr<TInt64ArrayMap> mapArr = std::make_shared<TInt64ArrayMap>(arr);
    int64_t& minVal = mapArr->min_;
    int64_t& maxVal = mapArr->max_;
    
    std::shared_ptr<arrow::Int64Array> numArr = std::static_pointer_cast<arrow::Int64Array>(arr);
    if (numArr != nullptr)
    {
      int64_t length = numArr->length();
      if (length >= 1)
      {
        minVal = numArr->Value(0);
        maxVal = numArr->Value(0);
      }
      for (int64_t i=1 ; i<length; i++)
      {
        const int64_t cv = numArr->Value(i);
        if (cv < minVal)
          minVal = cv;
        if (cv > maxVal)
          maxVal = cv;
      }
    }
    return mapArr;
  }

  std::shared_ptr<TColumnMap> TColumnMap::Make(std::shared_ptr<arrow::ChunkedArray> chunkedArray)
  {
    std::shared_ptr<TColumnMap> chunkArrMap = std::make_shared<TColumnMap>(chunkedArray);
    for (int64_t cnum=0; cnum<chunkedArray->num_chunks(); cnum++)
    {
      std::shared_ptr<arrow::Array> arr = chunkedArray->chunk(cnum);
      std::shared_ptr<TArrayMap> arrMap = TArrayMap::Make(arr);
      chunkArrMap->arrayMap_.push_back(arrMap);
    }
    return chunkArrMap;
  }
  
}
