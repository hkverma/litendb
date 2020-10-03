
#include <cstdint>
#include <iostream>
#include <vector>

#include "common.h"
#include "dtensor.h"
#include "TpchQueries.h"

using namespace tendb;

int main(int argc, char** argv) {

  if (argc < 2) {
    std::cout << "Usage: nvec file_name" << std::endl;
    return EXIT_SUCCESS;
  }
  std::string tpchDataDir = argv[1];

  // Initialize Google's logging library.
  google::InitGoogleLogging("tcache");

  std::shared_ptr<TCache>  tCache = std::make_shared<TCache>();

  TpchQueries tpchQueries(tCache, tpchDataDir);
  tpchQueries.ReadTables();

  StopWatch stopWatch;
  stopWatch.Start();
  double result = tpchQueries.Query6();
  stopWatch.Stop();
  std::cout << "Query6 Revenue=" << result << std::endl;
  std::cout << stopWatch.ElapsedInMicroseconds() << "us" << std::endl;

  stopWatch.Start();
  result = tpchQueries.Query5();
  stopWatch.Stop();
  std::cout << "Query5 Revenue=" << result << std::endl;
  std::cout << stopWatch.ElapsedInMicroseconds() << "us" << std::endl;

  stopWatch.Start();
  bool mapResult = tpchQueries.MakeMaps();
  stopWatch.Stop();
  std::cout << (mapResult?"Success":"Failed ") << " Column maps." << std::endl;
  std::cout << stopWatch.ElapsedInMicroseconds() << "us" << std::endl;

  stopWatch.Start();
  result = tpchQueries.Query5();
  stopWatch.Stop();
  std::cout << "Query5 Revenue=" << result << std::endl;
  std::cout << stopWatch.ElapsedInMicroseconds() << "us" << std::endl;
  
  return EXIT_SUCCESS;

}
