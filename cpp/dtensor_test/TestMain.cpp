
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

  std::string tpchDir = argv[1];

  // Get Cache and Demo objects
  std::shared_ptr<TCache>  tCache = TCache::GetInstance();
  // Read tpch tables into cache
  arrow::csv::ReadOptions readOptions = arrow::csv::ReadOptions::Defaults();
  readOptions.block_size = 1 << 20; // 1MB
  arrow::csv::ParseOptions parseOptions = arrow::csv::ParseOptions::Defaults();
  parseOptions.delimiter = '|';
  arrow::csv::ConvertOptions convertOptions = arrow::csv::ConvertOptions::Defaults();
  for (int32_t i=0; i<TpchDemo::tableNames.size(); i++)
  {
    std::string fileName = tpchDir + TpchDemo::tableNames[i] + ".tbl";
    auto ttable = tCache->ReadCsv(TpchDemo::tableNames[i], fileName,
                                  readOptions, parseOptions, convertOptions);
    //ttable->Print();
  }
  // tpchDemo->PrintSchemas();
  std::string cacheInfo = tCache->GetInfo();
  LOG(INFO) << "Cache=" << cacheInfo;
  
  // tpchDemo will probe cache to get all the tables
  shared_ptr<TpchDemo> tpchDemo = TpchDemo::GetInstance(tCache);


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

  auto printRevenue = [&](std::shared_ptr<std::unordered_map<std::string, double>> q5revs) -> void {
    LOG(INFO) << "Query5 Revenue=";
    for (auto it=q5revs->begin(); it!=q5revs->end(); it++) {
      LOG(INFO) << it->first << "=" << it->second;
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
