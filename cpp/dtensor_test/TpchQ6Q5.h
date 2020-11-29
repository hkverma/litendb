#include "dtensor.h"

namespace tendb
{

  class TpchQ6Q5
  {
  public:

    TpchQ6Q5(std::shared_ptr<TCache> tCache) : tCache_(tCache) { }

    // lineitem tables
    std::shared_ptr<arrow::ChunkedArray> lShipdate, lDiscount, lQuantity, lExtendedprice;    
    const static int32_t l_orderkey=0, l_suppkey=2, l_shipdate=10,
      l_discount=6, l_quantity=4, l_extendedprice=5;

    double Query6();
    void Query6Chunk(int64_t chunkNum, double& revenue);

    std::shared_ptr<TCache> tCache_;
    std::shared_ptr<TTable> lineitem_;

    int64_t date19970101Value;
    int64_t date19971231Value;
    
    // Num threads
    static const int64_t numParallels_ = 6;

  };

};
// These functions are exposed for external python like bindings
extern "C"
{
  double Tpch_Query6(tendb::TCache *tcache);
}
