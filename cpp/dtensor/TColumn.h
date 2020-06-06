//
// TenDB Columnar Storage Node
//
// TColumn wraps TColumnChunks to create a single column of TenDB structure
//
// While in memory, it is arrow. Persistent data is in Parquet.
//

#include <boost/uuid/uuid.hpp>

namespace tendb {

  class TColumn {
  public:
    arrow::Type::type type_;
    vector<shared_ptr<TColumnChunk>> columnChunkArray_;
    uint32_t totalColumnChunks_;
  }
};
