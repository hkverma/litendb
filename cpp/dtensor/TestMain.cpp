
#include <cstdint>
#include <iostream>
#include <vector>

#include <arrow/api.h>
#include <arrow/csv/api.h>
#include <arrow/filesystem/api.h>
#include <arrow/io/api.h>

#include "dtensor.h"
using namespace tendb;


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

  TCache tCache;
  std::shared_ptr<TTable> ttable = tCache.Read(fileName);
  ttable->Print();

  return EXIT_SUCCESS;
  
}
