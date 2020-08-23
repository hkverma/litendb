//
// TenDB Columnar Storage Node
//
// Columnar Data storage
//
// One column chunk stores numComponents of a single column of TenDB
// Arrow in memory, persistent in parquet format
//
#pragma once

#include <boost/uuid/uuid.hpp>
#include <arrow/api.h>
#include <vector>

namespace tendb {

  // Use UUID for each array
  // Abstract Base Array Map
  class TArrayMap {
    TArrayMap() { }
    arrow::Type type_;
  };

  // min max for integer, float, double date, tiemstamp time32
  template<class T>
  class TNumberArrayMap : public TArrayMap {
  public:
    T min;
    T max;
  };
};
