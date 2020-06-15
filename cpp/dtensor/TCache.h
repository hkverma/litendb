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
#include <unordered_map>
#include <boost/functional/hash.hpp>

// provide hash template with uuid
namespace std
{
  template<>
  struct hash<boost::uuids::uuid>
  {
    size_t operator () (const boost::uuids::uuid& uid) const
    {
        return boost::hash<boost::uuids::uuid>()(uid);
    }
  };
  
};

// TCache with hash templates
namespace tendb {

  class TCache {
  public:

    static std::shared_ptr<arrow::Array> GetArray(boost::uuids::uuid id);
    
    // For now the columnChunk should be present here. 
    // In future it could not be here, in that case fetch it if not present here
    static std::unordered_map<boost::uuids::uuid, std::shared_ptr<arrow::Array>> arrays_;

    static std::shared_ptr<arrow::Scalar> GetScalar(std::shared_ptr<arrow::Array> comp, int64_t rowNum);

  };

};
