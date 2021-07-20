#pragma once

#include <common.h>

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
  TColumnMap(std::shared_ptr<arrow::ChunkedArray> chunkedArray) :
    chunkedArray_(chunkedArray)
  {  }
  virtual ~TColumnMap() { }

  // Column Map from Arrow Chunked Array
  static std::shared_ptr<TColumnMap> Make(std::shared_ptr<arrow::ChunkedArray> chunkedArray);
  // Copy into another map structure
  virtual std::shared_ptr<TColumnMap> Copy();

  // TODO needs to be templated for min-max types
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

  // arrow chunked array for which map is built
  std::shared_ptr<arrow::ChunkedArray> chunkedArray_;
  //TODO arrow::Status status_;
};

// zone maps for comparable types currently only range type
// Create a MinMax template class here
// TODO Currently make it only for int64_t, enhance it later with other types
class TInt64ColumnMap : public TColumnMap
{
public:
  TInt64ColumnMap(std::shared_ptr<arrow::ChunkedArray> chunkedArray);
    
  static std::shared_ptr<TInt64ColumnMap> Make(std::shared_ptr<arrow::ChunkedArray> chunkedArray);

  virtual std::shared_ptr<TColumnMap> Copy();
    
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

  // min, max
  std::vector<int64_t> min_, max_;
  // Inverse maps from rowId to arrow::Array and offset within that array
  std::map<int64_t, std::pair<int64_t, int64_t>> reverseMap_;
    
};

  
}
