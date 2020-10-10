#include "dtensor.h"

#define CHECKFUNC(FUNC, MSG)       \
  do {                             \
    if (!FUNC)                     \
  {                                \
    LOG(INFO) << MSG ;             \
    break;                         \
  }                                \
  } while (0)

namespace tendb
{

  class TpchQueries
  {
  public:

    TpchQueries(std::shared_ptr<TCache> tCache, std::string& tpchDir) :
      tCache_(tCache), tpchDir_(tpchDir) { }

    // TPCH table enums
    enum {
      lineitem=0,
      customer=1,
      orders=2,
      supplier=3,
      nation=4,
      region=5,
      numTables=6
    };

    std::vector<std::string> tableNames =
      {
        "lineitem",
        "customer",
        "orders",
        "supplier",
        "nation",
        "region"
      };

    // Table entries
    const static int32_t c_custkey=0, c_nationkey=3;
    const static int32_t o_custkey=1, o_orderkey=0, o_orderdate=4;
    const static int32_t l_orderkey=0, l_suppkey=2, l_shipdate=10,
      l_discount=6, l_quantity=4, l_extendedprice=5;
    const static int32_t s_suppkey=0, s_nationkey=3;
    const static int32_t n_nationkey=0, n_regionkey=2, n_name=1;
    const static int32_t r_regionkey=0, r_name=1;

    void ReadTables();
    double Query6();
    double Query5();

    bool MakeMaps();
    
    void PrintSchemas();
    void PrintMaps();
    
    std::vector<std::shared_ptr<TTable>> tables_;
    std::shared_ptr<TCache> tCache_;
    std::string tpchDir_;

    const int64_t rowIncrementsForTimeLog = 100000;

  };

};
