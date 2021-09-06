#pragma once

#include <common.h>
#include <cache_fwd.h>

//
// Liten Columnar Storage Node
//
// Columnar Data storage
//
// One column chunk stores numComponents of a single column of Liten
// Arrow in memory, persistent in parquet format
//

namespace liten
{
//
// TODO Use Memory Pool to create an arrow memory pool liks  arrow::MemoryPool* pool_
//      Use pool to store all the values
//
class TColumnMap
{
public:
  /// Use Create to get the correct ColumnMap for the given type
  static TResult<std::shared_ptr<TColumnMap>> Create(std::shared_ptr<TColumn> tColumn);
  
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
  
  virtual bool GetReverseMap(int64_t& rowVal, int64_t& arrId, int64_t& rowId)
  {
    return false;
  }
  
  virtual bool GetReverseMap(std::stringstream& ss)
  {
    return false;
  }
  
  virtual bool IfValidMap()
  {
    return false;
  }
  
  /// Use named constructor Create instead
  TColumnMap(std::shared_ptr<TColumn> tColumn) :
    tColumn_(tColumn)
  {  }
  
  virtual ~TColumnMap() { }

private:
  
  /// arrow chunked array for which map is built
  std::shared_ptr<TColumn> tColumn_;

};

/// TBD Currently make it only for int64_t, enhance it later with other types

// zone maps for comparable types currently only range type
// Create a MinMax template class here
class TInt64ColumnMap : public TColumnMap
{
public:
    
  static TResult<std::shared_ptr<TInt64ColumnMap>> Create(std::shared_ptr<TColumn> tColumn);

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
  virtual bool IfValidMap()
  {
    return true;
  }

  virtual bool GetReverseMap(int64_t& rowVal, int64_t& arrId, int64_t& rowId);

  virtual bool GetReverseMap(std::stringstream& ss);

  /// Use named constructor Create instead
  TInt64ColumnMap(std::shared_ptr<TColumn> tColumn);

private:
  /// min, max
  std::vector<int64_t> min_, max_;
  
  /// Inverse maps from rowId to arrow::Array and offset within the array
  /// TBD can map to mulitple rows
  std::map<int64_t, std::pair<int64_t, int64_t>> reverseMap_;
    
};

  
}
