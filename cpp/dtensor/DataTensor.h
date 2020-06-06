//
// Catalog Data
//
// Represented in C++ class for cached data from MySQL
// Store in MySQL to provide consistent & persistent catalog nodes
//
//
// DataTensor is collection of columns.
//

namespace tendb {

  class DataTensor {
  public:
    arrow::Schema schema_;
    int numColumns__;
    vector<int> columnData_;
  };
};
