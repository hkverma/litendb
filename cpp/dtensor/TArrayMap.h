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

  // zone maps for integral
  template<class T>
  class TIntegralArrayMap : public TArrayMap {
  public:
    T min;
    T max;
  };
  
  // zone maps for float
  template<class T>
  class TFloatArrayMap : public TArrayMap {
  public:
    T min;
    T max;
  };
  
  // zone maps for date
  template<class T>
  class TDateArrayMap : public TArrayMap {
  public:
    T min;
    T max;
  };
  
};
