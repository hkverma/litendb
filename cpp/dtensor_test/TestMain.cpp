
#include <cstdint>
#include <iostream>
#include <vector>

#include "common.h"
#include "dtensor.h"
#include "TpchQueries.h"

using namespace tendb;

//
// TODO
//
// Async multi-tasking using Ray - explore
// CMakefile changes to put binarues in bin
// DSL - which ML algorithm to encode & which app
//
// Read TPCH data and print table
// Run Query 6
// Compare Query 6 against Spark on VM
// Do pre-join to create the tables
// Put zone-map for column chunks
//
// Run Query 5
// Compare Query 5 against Spark on VM
//
//
// Write DSLs - groupby scan filter etc.
// Write Query 6 & Query 5 using DSL
// Do TPCH queries using the DSL using jupyter
//
// Use arrow::Result to return Status along with the value
//


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


  return EXIT_SUCCESS;

}
