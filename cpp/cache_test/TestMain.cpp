
#include <cstdint>
#include <iostream>
#include <vector>
#include <filesystem>

#include <common.h>
#include <cache.h>
#include <TpchDemo.h>

#include <tbb/tbb.h>

using namespace liten;
using namespace std;

int main(int argc, char** argv) {

  TService::GetInstance()->Start();
  
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

  // Check paths here
  for (int32_t i=0; i<TpchDemo::tableNames.size(); i++)
  {
    std::filesystem::path fileName = tpchDir;
    std::string tableName = TpchDemo::tableNames[i] + ".tbl";
    fileName /= tableName;
    auto status = tCache->ReadCsv(TpchDemo::tableNames[i], TpchDemo::tableTypes[i], fileName,
                                  readOptions, parseOptions, convertOptions);
    if (!status.ok())
    {
      TLOG(ERROR) << "Unable to Read file=" << fileName;
      continue;
    }    
    auto ttable = tCache->GetTable(TpchDemo::tableNames[i]);
    if (nullptr == ttable)
    {
      TLOG(ERROR) << "Unable to get table from catalog=" << TpchDemo::tableNames[i];
      continue;
    }    
    //ttable->PrintSchema();
    //ttable->PrintTable();
  }

  // Create Schema Heirarchy
  auto lineitemSchema = tCache->GetSchema("lineitem_schema");
  auto customerSchema = tCache->GetSchema("customer_schema");
  auto ordersSchema = tCache->GetSchema("orders_schema");
  auto supplierSchema = tCache->GetSchema("supplier_schema");
  auto nationSchema = tCache->GetSchema("nation_schema");
  auto regionSchema = tCache->GetSchema("region_schema");
  if (nullptr == lineitemSchema || 
      nullptr == customerSchema ||
      nullptr == ordersSchema ||
      nullptr == supplierSchema ||
      nullptr == nationSchema ||
      nullptr == regionSchema)
  {
    TLOG(ERROR) << "Unable to get schemas";
  }
  else
  {
    lineitemSchema->Join("L_ORDERKEY", ordersSchema, "O_ORDERKEY");
    lineitemSchema->Join("L_SUPPKEY", supplierSchema, "S_SUPPKEY");
    ordersSchema->Join("O_CUSTKEY", customerSchema, "C_CUSTKEY");
    customerSchema->Join("C_NATIONKEY", nationSchema, "N_NATIONKEY");
    nationSchema->Join("N_REGIONKEY", regionSchema, "R_REGIONKEY");
  }
  
  std::string cacheInfo = tCache->GetInfo();
  LOG(INFO) << "Cache=" << cacheInfo;

  // tpchDemo will probe cache to get all the tables
  shared_ptr<TpchDemo> tpchDemo = TpchDemo::GetInstance(tCache);
  
  tpchDemo->PrintSchemas();

  // Run Query6
  auto execQuery6 = [&]()
  {
    TStopWatch stopWatch;
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
  };
  if (nullptr == tpchDemo->tables_[tpchDemo->lineitem])
  {
    LOG(ERROR) << "No lineitem table to run query 6";
  }
  else
  {
    execQuery6();
  }

  // Make Maps except for lineitem
  TStopWatch stopWatch;
  stopWatch.Start();
  for (int32_t i=1; i<TpchDemo::tableNames.size(); i++)
  {
    auto tableName = TpchDemo::tableNames[i];
    auto result = tCache->CreateMaps(tableName);
    if (!result.ok())
    {
      LOG(ERROR) << "Failed to create maps for " << tableName << "with msg=" << result.message();
    }
    else
    {
      LOG(INFO) << "Success create maps for " << tableName;
    }
  }
  stopWatch.Stop();
  LOG(INFO) << "Liten Tensor Creation Time (us)=" << stopWatch.ElapsedInMicroseconds();
  tpchDemo->PrintMaps(tpchDemo->customer);

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
