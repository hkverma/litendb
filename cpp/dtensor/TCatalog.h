//
// Catalog Data
//
// Represented in C++ class for cached data from MySQL
// Store in MySQL to provide consistent & persistent catalog nodes
//
// Catalog is represented by a vector of rows. It is a flat representation of Data Tensor.
// TBD
// Stored as a MySQL table for persistence. 
// TCatalog is prepared from MySQL database. Any updated in MySQL should be updated to TCatalog.
//
#pragma once

#include <boost/uuid/uuid.hpp>
#include <arrow/api.h>
#include <vector>
#include <map>
#include "TColumnChunk.h"

namespace tendb {

  class TCatalogComponent {
    boost::uuids::uuid tColumnChunkUuid_;
    int tColumnChunkSequenceNum_;
    int tColumnNum_;
    int tStartOffset;
  };
  
  class TCatalog {
  public:
    void Init(std::shared_ptr<arrow::Table> table);
    std::map<boost::uuids::uuid, std::shared_ptr<TColumnChunk>> toColumnChunks_;
    std::shared_ptr<arrow::Schema> schema_;
    std::vector<std::shared_ptr<TCatalogComponent>> components_;
  };
  
};
