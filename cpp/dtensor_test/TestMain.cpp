
#include <cstdint>
#include <iostream>
#include <vector>

#include "common.h"
#include "dtensor.h"
#include "TpchQueries.h"

#include <tbb/tbb.h>

using namespace tendb;

int main(int argc, char** argv) {

  /*
  tbb::parallel_invoke
    ([]() { std::cout << "Hello " << std::endl; },
     []() { std::cout << "Hello " << std::endl; }
     );
  return 0;
  */

  if (argc < 2) {
    std::cout << "Usage: exec file_name" << std::endl;
    return EXIT_SUCCESS;
  }
  std::string tpchDataDir = argv[1];

  // Initialize Google's logging library.
  google::InitGoogleLogging("tendb");

  std::shared_ptr<TCache>  tCache = std::make_shared<TCache>();

  TpchQueries tpchQueries(tCache, tpchDataDir);
  tpchQueries.ReadTables();
  tpchQueries.PrintSchemas();

  StopWatch stopWatch;
  stopWatch.Start();
  double result = tpchQueries.Query6Serial();
  stopWatch.Stop();
  LOG(INFO) << "Query6 Revenue=" << result;
  LOG(INFO) << stopWatch.ElapsedInMicroseconds() << "us";

  stopWatch.Start();
  result = tpchQueries.Query6Parallel();
  stopWatch.Stop();
  LOG(INFO) << "Query6 Revenue=" << result;
  LOG(INFO) << stopWatch.ElapsedInMicroseconds() << "us";
  
  stopWatch.Start();
  bool mapResult = tpchQueries.MakeMaps();
  stopWatch.Stop();
  LOG(INFO) << (mapResult?"Success":"Failed ") << " Column maps.";
  LOG(INFO) << "TenDB Tensor Creation Time (us)=" << stopWatch.ElapsedInMicroseconds();
  tpchQueries.PrintMaps();

  double revenue[25] = {0};
  auto printRevenue = [&]() -> void {
    const char* nations[25] = {
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
    int64_t russia=22, romania=19, uk=23, france=6, germany=7;
    LOG(INFO) << "Query5 Revenue=";
    LOG(INFO) << nations[russia] << "=" << revenue[russia];
    LOG(INFO) << nations[romania] << "=" << revenue[romania];
    LOG(INFO) << nations[uk] << "=" << revenue[uk];
    LOG(INFO) << nations[france] << "=" << revenue[france];
    LOG(INFO) << nations[germany] << "=" << revenue[germany];

    LOG(INFO) << "All nations Revenue=";
    for (int i=0 ; i< 25; i++) {
      LOG(INFO) << nations[i] << "=" << revenue[i];
    }
  };

  stopWatch.Start();
  tpchQueries.Query5Serial(revenue);
  stopWatch.Stop();
  printRevenue();
  LOG(INFO) << stopWatch.ElapsedInMicroseconds() << "us";

  for (int i=0; i<25; i++)
    revenue[i] = 0;
  stopWatch.Start();
  tpchQueries.Query5Parallel(revenue);
  stopWatch.Stop();
  printRevenue();
  LOG(INFO) << stopWatch.ElapsedInMicroseconds() << "us";

  return EXIT_SUCCESS;

}
