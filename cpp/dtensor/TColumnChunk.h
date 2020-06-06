//
// TenDB Columnar Storage Node
//
// Columnar Data storage
//
// One column chunk stores numComponents of a single column of TenDB
// Arrow in memory, persistent in parquet format
//

#include <boost/uuid/uuid.hpp>


namespace tendb {

  class TColumnChunk {
  public:
    arrow::Type::type type_;
    uint32_t totalComponents_;
    boost::uuids::uuid uuid_;
    
  };

};
