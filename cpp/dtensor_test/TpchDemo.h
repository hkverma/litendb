#include "dtensor.h"

namespace tendb
{

  // TODO expose numWorkers to the demo same as from TCache

  class TpchDemo
  {
  public:

    static std::shared_ptr<TpchDemo> GetInstance(std::shared_ptr<TCache> tCache);
    TpchDemo(std::shared_ptr<TCache> tCache);

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
    static const std::vector<std::string> tableNames;

    // Table entries
    const static int32_t c_custkey=0, c_nationkey=3;
    const static int32_t o_custkey=1, o_orderkey=0, o_orderdate=4;
    const static int32_t l_orderkey=0, l_suppkey=2, l_shipdate=10,
      l_discount=6, l_quantity=4, l_extendedprice=5;
    const static int32_t s_suppkey=0, s_nationkey=3;
    const static int32_t n_nationkey=0, n_regionkey=2, n_name=1;
    const static int32_t r_regionkey=0, r_name=1;

    // lineitem tables
    std::shared_ptr<arrow::ChunkedArray> lShipdate, lDiscount, lQuantity, lExtendedprice;
    std::shared_ptr<arrow::ChunkedArray> lOrderkey, lSuppkey;

    // Read tables for Query6 and Query5
    void ReadTables(std::string tpchDir);

    // Run Query6
    double Query6();
    double Query6Serial();
    // Get Query6 revenue for chunk chunkNum
    void GetQuery6Revenue(int64_t chunkNum, double& revenue);

    // Run Query5
    std::shared_ptr<std::unordered_map<std::string, double>> Query5();
    std::shared_ptr<std::unordered_map<std::string, double>> Query5Serial();
    // Get Query5 revenue for chunk chunkNum
    void GetQuery5Revenue(int64_t chunkNum, double revenue[], int32_t mapNum);

    void PrintSchemas();
    void PrintMaps(int startAt=lineitem);

    std::vector<std::shared_ptr<TTable>> tables_;
    std::shared_ptr<TCache> tCache_;

    const int64_t rowIncrementsForTimeLog = 500000;
    int64_t date19970101Value;
    int64_t date19971231Value;
    int64_t date19950101Value;
    int64_t date19951231Value;

    static std::shared_ptr<TpchDemo> tpchDemo_;

    // Total worker numbers
    static const int64_t numWorkers_ = 6;
    static const int64_t numMaps_ = 1;

    // Store TpchData properties
    static const int numNations = 25;
    inline static const std::string nations[numNations] = {
      "ALGERIA",
      "ARGENTINA",
      "BRAZIL",
      "CANADA",
      "EGYPT",
      "ETHIOPIA",
      "FRANCE",
      "GERMANY",
      "INDIA",
      "INDONESIA",
      "IRAN",
      "IRAQ",
      "JAPAN",
      "JORDAN",
      "KENYA",
      "MOROCCO",
      "MOZAMBIQUE",
      "PERU",
      "CHINA",
      "ROMANIA",
      "SAUDI ARABIA",
      "VIETNAM",
      "RUSSIA",
      "UNITED KINGDOM",
      "UNITED STATES"
    };
    // Query5 revenues
    double q5revenues[25];
    void ClearQ5Revenues();
    std::shared_ptr<std::unordered_map<std::string, double>> GetAggrRevenues();
    // Utility functions
    void InitTpchTables();
  };

};
