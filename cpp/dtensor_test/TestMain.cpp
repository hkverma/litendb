
#include <cstdint>
#include <iostream>
#include <vector>

#include "common.h"
#include "dtensor.h"
#include "TpchDemo.h"

#include <tbb/tbb.h>

using namespace tendb;
using namespace std;

int main(int argc, char** argv) {

  if (argc < 2) {
    std::cout << "Usage: exec file_name" << std::endl;
    return EXIT_SUCCESS;
  }
  std::string tpchDataDir = argv[1];

  // Get Cache and Demo objects
  std::shared_ptr<TCache>  tCache = TCache::GetInstance();
  shared_ptr<TpchDemo> tpchDemo = TpchDemo::GetInstance(tCache);

  // Read tpch tables
  tpchDemo->ReadTables(tpchDataDir);
  tpchDemo->PrintSchemas();

  // Run Query6
  StopWatch stopWatch;
  stopWatch.Start();
  double result = tpchDemo->Query6Serial();
  stopWatch.Stop();
  LOG(INFO) << "Query6 Revenue=" << result;
  LOG(INFO) << stopWatch.ElapsedInMicroseconds() << "us";

  stopWatch.Start();
  result = tpchDemo->Query6();
  stopWatch.Stop();
  LOG(INFO) << "Query6 Revenue=" << result;
  LOG(INFO) << stopWatch.ElapsedInMicroseconds() << "us";

  stopWatch.Start();
  bool mapResult = tpchDemo->MakeMaps();
  stopWatch.Stop();
  LOG(INFO) << (mapResult?"Success":"Failed ") << " Column maps.";
  LOG(INFO) << "TenDB Tensor Creation Time (us)=" << stopWatch.ElapsedInMicroseconds();
  tpchDemo->PrintMaps();

  auto printRevenue = [&](std::map<std::string, double>& q5revs) -> void {
    LOG(INFO) << "Query5 Revenue=";
    for (auto nrev: q5revs) {
      LOG(INFO) << nrev.first << "=" << nrev.second;
    }
  };

  stopWatch.Start();
  auto q5revs = tpchDemo->Query5();
  stopWatch.Stop();
  printRevenue(q5revs);
  LOG(INFO) << stopWatch.ElapsedInMicroseconds() << "us";

  stopWatch.Start();
  q5revs = tpchDemo->Query5Serial();
  stopWatch.Stop();
  printRevenue(q5revs);
  LOG(INFO) << stopWatch.ElapsedInMicroseconds() << "us";

  return EXIT_SUCCESS;

}
