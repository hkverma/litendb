
#include <cstdint>
#include <iostream>
#include <vector>

#include "common.h"
#include "dtensor.h"
#include "TpchQueries.h"

using namespace tendb;

int main(int argc, char** argv) {

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
  double result = tpchQueries.Query6();
  stopWatch.Stop();
  LOG(INFO) << "Query6 Revenue=" << result;
  LOG(INFO) << stopWatch.ElapsedInMicroseconds() << "us";

  /*
  stopWatch.Start();
  result = tpchQueries.Query5);
  stopWatch.Stop();
  LOG(INFO) << "Query5 Revenue=" << result;
  LOG(INFO) << stopWatch.ElapsedInMicroseconds() << "us";
  */
  stopWatch.Start();
  bool mapResult = tpchQueries.MakeMaps();
  stopWatch.Stop();
  LOG(INFO) << (mapResult?"Success":"Failed ") << " Column maps.";
  LOG(INFO) << stopWatch.ElapsedInMicroseconds() << "us";
  tpchQueries.PrintMaps();

  stopWatch.Start();
  result = tpchQueries.Query5();
  stopWatch.Stop();
  LOG(INFO) << "Query5 Revenue=" << result;
  LOG(INFO) << stopWatch.ElapsedInMicroseconds() << "us";
  
  return EXIT_SUCCESS;

}
