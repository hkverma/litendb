#pragma once

#include <common.h>
#include <cache_fwd.h>
//#include <TColumn.h>

//
// Liten Columnar Storage Node
//
// Columnar Data storage
//
// One column chunk stores numComponents of a single column of Liten
// Arrow in memory, persistent in parquet format
//
// TBD Use Arrow Array to store Column Map data
//

namespace liten
{
//
// TODO Use Memory Pool to create an arrow memory pool liks  arrow::MemoryPool* pool_
//      Use pool to store all the values
class TColumnMap
{
public:


  virtual TStatus CreateZoneMap(bool forceCreate=false)
  {
    ifZoneMap_ = false;
    return TStatus::OK();
  }
  
  virtual TStatus CreateReverseMap(bool forceCreate=false)
  {
    ifReverseMap_ = false;
    return TStatus::OK();
  }
  
  // TBD needs to be templated for min-max types
  virtual bool GetMin(int64_t arrNum, int64_t& minVal)
  {
    return false;
  }

  virtual bool GetMax(int64_t arrNum, int64_t& maxVal)
  {
    return false;
  }

  virtual bool GetRowId(int64_t& rowId, int64_t& rowVal)
  {
    return false;
  }
  
  virtual TRowId GetReverseMap(int64_t& rowVal);
  
  virtual bool GetReverseMap(std::stringstream& ss)
  {
    return false;
  }
  
  virtual bool IfValidZoneMap()
  {
    return false;
  }
  
  virtual bool IfValidReverseMap()
  {
    return false;
  }
  
  /// Columnmap for a given column
  TColumnMap(std::shared_ptr<TColumn> tColumn) :
    tColumn_(tColumn), ifZoneMap_(false), ifReverseMap_(false)
  {  }
  
  virtual ~TColumnMap() { }

protected:

  // TBD Can only be created from TColumn
  friend class TColumn;
  static TResult<std::shared_ptr<TColumnMap>> Create(std::shared_ptr<TColumn> tColumn);
  
  /// arrow chunked array for which map is built
  std::shared_ptr<TColumn> tColumn_;

  /// Create zoneMap if true
  bool ifZoneMap_;

  /// Create reverseMap if true
  bool ifReverseMap_;

};

/// TBD Currently make it only for int64_t, enhance it later with other types

// zone maps for comparable types currently only range type
// Create a MinMax template class here
class TInt64ColumnMap : public TColumnMap
{
public:
    
  /// Use named constructor Create instead
  TInt64ColumnMap(std::shared_ptr<TColumn> tColumn);
  
  virtual TStatus CreateZoneMap(bool forceCreate=false);
  
  virtual TStatus CreateReverseMap(bool forceCreate=false);
  
  virtual bool GetMin(int64_t arrNum, int64_t& minVal)
  {
    minVal = min_[arrNum];
    return true;
  }

  virtual bool GetMax(int64_t arrNum, int64_t& maxVal)
  {
    maxVal = max_[arrNum];
    return true;
  }
  virtual bool IfValidZoneMap()
  {
    return ifZoneMap_;
  }

  virtual bool IfValidReverseMap()
  {
    return ifReverseMap_;
  }
  
  virtual TRowId GetReverseMap(int64_t& rowVal);

  virtual bool GetReverseMap(std::stringstream& ss);


private:
  /// min, max
  std::vector<int64_t> min_, max_;
  
  /// Inverse maps from rowId to arrow::Array and offset within the array
  /// TBD can map to mulitple rows
  std::map<int64_t, std::pair<int64_t, int64_t>> reverseMap_;
};

  
}
