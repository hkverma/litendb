
#include <cstdint>
#include <iostream>
#include <vector>

#include <arrow/api.h>
#include <arrow/csv/api.h>
#include <arrow/filesystem/api.h>
#include <arrow/io/api.h>

#include <glog/logging.h>
#include "dtensor.h"
using namespace tendb;

//
// TODO
//
// Read TPCH data and print table
// Run Query 6
// Compare Query 6 against Spark on VM
// Write join tables
// Run Query 5
// Compare Query 5 against Spark on VM
// Write DSLs - groupby scan filter etc.
// Write Query 6 & Query 5 using DSL
// Do TPCH queries using the DSL using jupyter
//
// Use arrow::Result to return Status along with the value
//

#define EXIT_ON_FAILURE(expr)                      \
  do {                                             \
    arrow::Status status_ = (expr);                \
    if (!status_.ok()) {                           \
      std::cerr << status_.message() << std::endl; \
      return EXIT_FAILURE;                         \
    }                                              \
  } while (0);


int main(int argc, char** argv) {

  if (argc < 2) {
    std::cout << "Usage: nvec file_name" << std::endl;
    return EXIT_SUCCESS;
  }
  std::string fileName = argv[1];

  // Initialize Google's logging library.
  google::InitGoogleLogging("tcache");
     
  TCache tCache;
  
  arrow::csv::ReadOptions readOptions = arrow::csv::ReadOptions::Defaults();
  arrow::csv::ParseOptions parseOptions = arrow::csv::ParseOptions::Defaults();
  parseOptions.delimiter = '|';
  arrow::csv::ConvertOptions convertOptions = arrow::csv::ConvertOptions::Defaults();
  std::string tableName = "LINEITEM";
  
  std::shared_ptr<TTable> ttable = tCache.ReadCsv(tableName, fileName,
                                                  readOptions, parseOptions, convertOptions);
  ttable->Print();

  return EXIT_SUCCESS;
  
}
