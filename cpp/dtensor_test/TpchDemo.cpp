#include <cstdint>
#include <iostream>
#include <vector>

#include "common.h"
#include "dtensor.h"

#include "TpchDemo.h"
#include <tbb/tbb.h>

using namespace tendb;

// Tables
const std::vector<std::string> TpchDemo::tableNames =
  {
    "lineitem",
    "customer",
    "orders",
    "supplier",
    "nation",
    "region"
  };

// Maintain a tpchDemo classs
std::shared_ptr<TpchDemo> TpchDemo::tpchDemo_ = nullptr;
std::shared_ptr<TpchDemo> TpchDemo::GetInstance(std::shared_ptr<TCache> tCache)
{
  if (tpchDemo_ == nullptr)
  {
    tpchDemo_ = std::make_shared<TpchDemo>(tCache);
  }
  tpchDemo_->InitTpchTables();
  return tpchDemo_;
}

TpchDemo::TpchDemo(std::shared_ptr<TCache> tCache) :
      tCache_(tCache) {
  // constants
  date19970101Value =
    SecondsSinceEpoch(boost::gregorian::date(1997, 1, 1), boost::posix_time::seconds(0));
  date19971231Value =
    SecondsSinceEpoch(boost::gregorian::date(1997, 12, 31), boost::posix_time::seconds(0));
  date19950101Value =
    SecondsSinceEpoch(boost::gregorian::date(1995, 1, 1), boost::posix_time::seconds(0));
  date19951231Value =
    SecondsSinceEpoch(boost::gregorian::date(1995, 12, 31), boost::posix_time::seconds(0));
}


void TpchDemo::ReadTables(std::string tpchDir)
{
  arrow::csv::ReadOptions readOptions = arrow::csv::ReadOptions::Defaults();
  readOptions.block_size = 1 << 20; // 1MB
  arrow::csv::ParseOptions parseOptions = arrow::csv::ParseOptions::Defaults();
  parseOptions.delimiter = '|';
  arrow::csv::ConvertOptions convertOptions = arrow::csv::ConvertOptions::Defaults();

  tables_.resize(numTables);

  for (int32_t i=0; i<numTables; i++)
  {
    std::string fileName = tpchDir + tableNames[i] + ".tbl";
    tables_[i] = tCache_->ReadCsv(tableNames[i], fileName,
                                  readOptions, parseOptions, convertOptions);
    //tables_[i]->Print();
  }

}

// Do these before running query, table may have changed
void TpchDemo::InitTpchTables()
{
  tables_.resize(numTables);
  for (int32_t i=0; i<numTables; i++)
  {
    auto ttable = tCache_->GetTable(tableNames[i]);
    if (nullptr == ttable)
    {
      LOG(INFO) << "No table " << tableNames[i] << " in cache";
    }
    else
    {
      LOG(INFO) << "Found table " << tableNames[i] << " in cache";
    }
    tables_[i] = ttable;
  }

  if (nullptr == tables_[lineitem])
  {
    LOG(ERROR) << "No lineitem table";
  }
  else
  {
    // Populate lineitem columns
    lShipdate = tables_[lineitem]->GetTable()->column(l_shipdate);
    lDiscount = tables_[lineitem]->GetTable()->column(l_discount);
    lQuantity = tables_[lineitem]->GetTable()->column(l_quantity);
    lExtendedprice = tables_[lineitem]->GetTable()->column(l_extendedprice);
    lOrderkey = tables_[lineitem]->GetTable()->column(l_orderkey);
    lSuppkey = tables_[lineitem]->GetTable()->column(l_suppkey);
  }
}

/*
select
	sum(l_extendedprice * l_discount) as revenue
from
	lineitem
where
	l_shipdate >= date '1997-01-01'
	and l_shipdate < date '1997-01-01' + interval '1' year
	and l_discount between 0.07 - 0.01 and 0.07 + 0.01
	and l_quantity < 25;
*/
double TpchDemo::Query6Serial()
{
  if (tables_[lineitem] == nullptr)
  {
    LOG(ERROR) <<  "No valid table to run Query6" ;
    return 0;
  }
  int shipdateChunkNum=0, discountChunkNum=0, quantityChunkNum=0, extendedpriceChunkNum=0;
  TColumnIterator<int64_t, arrow::Int64Array> shipdateIter(lShipdate);
  TColumnIterator<double, arrow::DoubleArray> discountIter(lDiscount);
  TColumnIterator<int64_t, arrow::Int64Array> quantityIter(lQuantity);
  TColumnIterator<double, arrow::DoubleArray> extendedpriceIter(lExtendedprice);

  int64_t length = lShipdate->length();
  if (length != lDiscount->length() ||
      length != lQuantity->length() ||
      length != lExtendedprice->length())
  {
    LOG(ERROR) << "Length should be the same";
    return 0;
  }

  double revenue = 0;
  int64_t shipdateValue, quantityValue;
  double discountValue, extendedpriceValue;

  // for now do a full table scan need to build filtering metadata per column chunk
  for (int64_t rowId=0; rowId<length; rowId++)
  {
    if (!shipdateIter.next(shipdateValue)) break;
    if (!discountIter.next(discountValue)) break;
    if (!quantityIter.next(quantityValue)) break;
    if (!extendedpriceIter.next(extendedpriceValue)) break;
    if (shipdateValue < date19970101Value || shipdateValue > date19971231Value)
      continue;
    if (quantityValue >= 25)
      continue;
    if (discountValue < 0.06 || discountValue > 0.08)
      continue;
    revenue += discountValue * extendedpriceValue;
  }
  return revenue;
}

void TpchDemo::GetQuery6Revenue(int64_t chunkNum, double& revenue)
{
  auto shipdate = std::static_pointer_cast<arrow::Int64Array>(lShipdate->chunk(chunkNum));
  auto discount = std::static_pointer_cast<arrow::DoubleArray>(lDiscount->chunk(chunkNum));
  auto quantity = std::static_pointer_cast<arrow::Int64Array>(lQuantity->chunk(chunkNum));
  auto extendedprice = std::static_pointer_cast<arrow::DoubleArray>(lExtendedprice->chunk(chunkNum));
  revenue = 0;

  for (int64_t rowNum=0; rowNum<extendedprice->length(); rowNum++)
  {
    auto shipdateValue = shipdate->Value(rowNum);
    if (shipdateValue < date19970101Value || shipdateValue > date19971231Value)
      continue;
    auto quantityValue = quantity->Value(rowNum);
    if (quantityValue >= 25)
      continue;
    auto discountValue = discount->Value(rowNum);
    if (discountValue < 0.06 || discountValue > 0.08)
      continue;
    auto extendedpriceValue = extendedprice->Value(rowNum);
    revenue += discountValue * extendedpriceValue;
  }

};

// Get Query6 Revenue, use numWorkers_ threads
double TpchDemo::Query6()
{
  int64_t shipdateValue, quantityValue;
  double discountValue, extendedpriceValue;
  if (tables_[lineitem] == nullptr)
  {
    LOG(ERROR) <<  "No valid table to run Query6" ;
    return 0;
  }

  tbb::task_group tg;

  StopWatch timer;
  timer.Start();

  return 1000;
  // for now do a full table scan need to build filtering metadata per column chunk
  int64_t numChunks = lExtendedprice->num_chunks();
  std::vector<double> revenues(lExtendedprice->num_chunks());

  int64_t pnum=0;
  for (int64_t chunkNum = 0; chunkNum < numChunks; chunkNum++)
  {
    auto tf = std::bind(&TpchDemo::GetQuery6Revenue, this, chunkNum, std::ref(revenues[chunkNum]));
    tg.run(tf);
    pnum++;
    if (pnum == numWorkers_)
    {
      tg.wait();
      pnum = 0;
    }
  }
  if (pnum > 0)
    tg.wait();

  double revenue = 0;
  for (auto rev: revenues)
  {
    revenue += rev;
  }
  return revenue;
}


inline void TpchDemo::ClearQ5Revenues()
{
  for (int i=0;i<numNations;i++)
  {
    q5revenues[i]=0;
  }
}

inline std::map<std::string, double> TpchDemo::GetAggrRevenues()
{
  std::map<std::string, double> result;
  for (int i=0;i<numNations;i++)
  {
    if (q5revenues[i]>0) {
      result[nations[i]] = q5revenues[i];
    }
  }
  return (move(result));
}

/*
select
	n_name,
	sum(l_extendedprice * (1 - l_discount)) as revenue
from
	customer,
	orders,
	lineitem,
	supplier,
	nation,
	region
where
	c_custkey = o_custkey
	and l_orderkey = o_orderkey
	and l_suppkey = s_suppkey
	and c_nationkey = s_nationkey
	and s_nationkey = n_nationkey
	and n_regionkey = r_regionkey
	and r_name = 'EUROPE'
	and o_orderdate >= date '1995-01-01'
	and o_orderdate < date '1995-01-01' + interval '1' year
group by
	n_name
order by
	revenue desc;
limit -1;
*/
std::map<std::string, double> TpchDemo::Query5Serial()
{
  if (nullptr == tables_[lineitem] || nullptr == tables_[supplier] ||
      nullptr == tables_[orders] || nullptr == tables_[customer] ||
      nullptr == tables_[nation] || nullptr == tables_[region])
  {
    LOG(ERROR) <<  "No valid table to run Query5" ;
    return std::map<std::string, double>();
  }

  TColumnIterator<double, arrow::DoubleArray> lDiscountIter(lDiscount);
  TColumnIterator<double, arrow::DoubleArray> lExtendedpriceIter(lExtendedprice);
  TColumnIterator<int64_t, arrow::Int64Array> lOrderkeyIter(lOrderkey);
  TColumnIterator<int64_t, arrow::Int64Array> lSuppkeyIter(lSuppkey);

  std::map<std::string, double> q5result;

  int64_t length = lDiscount->length();
  if (length != lExtendedprice->length())
  {
    LOG(ERROR) << "Length should be the same";
    return q5result;
  }

  ClearQ5Revenues();

  int64_t orderRowId, oOrderdateValue;
  int64_t suppRowId, sNationkeyValue;
  int64_t nationRowId;
  int64_t nRegionkeyValue;
  int64_t regionRowId;
  //arrow::util::string_view rNameValue;
  std::string rNameValue;
  int64_t lOrderkeyValue, lSuppkeyValue;
  double lDiscountValue, lExtendedpriceValue;

  std::string europe("EUROPE");
  std::string rNationValue;


  StopWatch timer;
  timer.Start();

  // for now do a full table scan need to build filtering metadata per column chunk
  StopWatch totalTimer;
  totalTimer.Start();
  StopWatch extraTimer;
  extraTimer.Start();
  StopWatch scanTimer;
  scanTimer.Start();

  int64_t ordersGetRowIdTime=0, ordersGetValTime=0, extraTime=0, scanTime=0, totalTime=0;

  int64_t supplierGetRowIdTime=0, supplierGetValTime=0;
  int64_t nationGetRowIdTime=0, nationGetValTime=0;

  bool countExtraTime = true;
  for (int64_t rowId=0; rowId<length; rowId++)
  {
    if (countExtraTime)
    {
      extraTimer.Stop();
      extraTime += extraTimer.ElapsedInMicroseconds();
      countExtraTime = false;
    }

    scanTimer.Stop();
    scanTime += scanTimer.ElapsedInMicroseconds();

    if (rowId%rowIncrementsForTimeLog == 0)
    {
      totalTimer.Stop();
      totalTime += totalTimer.ElapsedInMicroseconds();
      totalTimer.Start();
      LOG(INFO) << "Rows = " << rowId << " Elapsed ms=" << totalTime/1000;
      LOG(INFO) << "Orders RowId Time ms= " << ordersGetRowIdTime/1000;
      LOG(INFO) << "Orders ValId Time ms= " << ordersGetValTime/1000;
      LOG(INFO) << "Supplier RowId Time ms= " << supplierGetRowIdTime/1000;
      LOG(INFO) << "Supplier ValId Time ms= " << supplierGetValTime/1000;
      LOG(INFO) << "Nation RowId Time ms= " << nationGetRowIdTime/1000;
      LOG(INFO) << "Nation ValId Time ms= " << nationGetValTime/1000;
      LOG(INFO) << "Scan Time ms= " << scanTime/1000;
      LOG(INFO) << "Extra Time ms= " << extraTime/1000;
    }

    scanTimer.Start();
    // Get all values for the row first
    if (!lOrderkeyIter.next(lOrderkeyValue))
    {
      LOG(ERROR) << "Missing order key" ;
      break;
    }
    if (!lSuppkeyIter.next(lSuppkeyValue))
    {
      LOG(ERROR) << "Missing supply key" ;
      break;
    }
    if (!lExtendedpriceIter.next(lExtendedpriceValue))
    {
      LOG(ERROR) << "Missing extended price key" ;
      break;
    }
    if (!lDiscountIter.next(lDiscountValue))
    {
      LOG(ERROR) << "Missing discount Value" ;
      break;
    }
    scanTimer.Stop();

    countExtraTime = true;
    extraTimer.Start();
    // l_orderkey = o_orderkey
    // and o_orderdate >= date '1995-01-01'
    // and o_orderdate < date '1995-01-01' + interval '1' year
    if (!JoinInner<int64_t, arrow::Int64Array>
        (tables_[orders],
         lOrderkeyValue, o_orderkey, ordersGetRowIdTime,
         oOrderdateValue, o_orderdate, ordersGetValTime, 0))
      continue;
    if (oOrderdateValue < date19950101Value || oOrderdateValue > date19951231Value)
      continue;

    // Filter on r_name
    // l_suppkey = s_suppkey,
    if ( !JoinInner<int64_t, arrow::Int64Array>
         (tables_[supplier],
          lSuppkeyValue, s_suppkey, supplierGetRowIdTime,
          sNationkeyValue, s_nationkey, supplierGetValTime, 0))
      continue;

    // s_nationkey = n_nationkey also get nation name
    if ( !JoinInner<int64_t, arrow::Int64Array>
         (tables_[nation],
          sNationkeyValue, n_nationkey, nationGetRowIdTime,
          nRegionkeyValue, n_regionkey, nationGetValTime, 0))
      continue;

    // n_regionkey = r_regionkey
    if (nRegionkeyValue != 3)
      continue;

    /* Comparing of the names are not needed
    if (!(GetRowId<int64_t, arrow::Int64Array>(regionRowId, nRegionkeyValue, tables_[region], r_regionkey))) continue;
    // if (!(GetValue<arrow::util::string_view, arrow::StringArray>(regionRowId, rNameValue, rName))) continue;
    if (!(GetValue<std::string, arrow::StringArray>(regionRowId, rNameValue, tables_[region], r_name))) continue;

    // r_name = 'EUROPE';
    bool ifEurope = std::equal(europe.begin(), europe.end(), rNameValue.begin(),
                                 [] (const char& a, const char& b)
                                 {
                                   return (std::tolower(a) == std::tolower(b));
                                 });
    if (!ifEurope)
      continue;
    */

    // add to revenue by nation key
    q5revenues[sNationkeyValue] += (1-lDiscountValue)*lExtendedpriceValue;
  }

  timer.Stop();
  LOG(INFO) << "Query 5 Elapsed ms=" << timer.ElapsedInMicroseconds()/1000;
  q5result = GetAggrRevenues();
  return (move(q5result));
}

void TpchDemo::GetQuery5Revenue(int64_t chunkNum, double revenue[], int32_t mapNum)
{
  auto orderkey = std::static_pointer_cast<arrow::Int64Array>(lOrderkey->chunk(chunkNum));
  auto suppkey = std::static_pointer_cast<arrow::Int64Array>(lSuppkey->chunk(chunkNum));
  auto discount = std::static_pointer_cast<arrow::DoubleArray>(lDiscount->chunk(chunkNum));
  auto quantity = std::static_pointer_cast<arrow::Int64Array>(lQuantity->chunk(chunkNum));
  auto extendedprice = std::static_pointer_cast<arrow::DoubleArray>(lExtendedprice->chunk(chunkNum));
  StopWatch timer;
  timer.Start();

  int64_t ordersGetRowIdTime=0, ordersGetValTime=0, extraTime=0, scanTime=0, totalTime=0;
  int64_t supplierGetRowIdTime=0, supplierGetValTime=0;
  int64_t nationGetRowIdTime=0, nationGetValTime=0;

  int64_t oOrderdateValue, sNationkeyValue, nRegionkeyValue;
  int64_t rowId;
  for (rowId=0; rowId<extendedprice->length(); rowId++)
  {
    // Get all values for the row first
    auto lOrderkeyValue = orderkey->Value(rowId);
    auto lSuppkeyValue = suppkey->Value(rowId);
    auto lExtendedpriceValue = extendedprice->Value(rowId);
    auto lDiscountValue = discount->Value(rowId);

    // l_orderkey = o_orderkey
    // and o_orderdate >= date '1995-01-01'
    // and o_orderdate < date '1995-01-01' + interval '1' year
    if (!JoinInner<int64_t, arrow::Int64Array>
        (tables_[orders],
         lOrderkeyValue, o_orderkey, ordersGetRowIdTime,
         oOrderdateValue, o_orderdate, ordersGetValTime,
         mapNum))
      continue;
    if (oOrderdateValue < date19950101Value || oOrderdateValue > date19951231Value)
      continue;

    // Filter on r_name
    // l_suppkey = s_suppkey,
    if ( !JoinInner<int64_t, arrow::Int64Array>
         (tables_[supplier],
          lSuppkeyValue, s_suppkey, supplierGetRowIdTime,
          sNationkeyValue, s_nationkey, supplierGetValTime,
          mapNum))
      continue;

    // s_nationkey = n_nationkey also get nation name
    if ( !JoinInner<int64_t, arrow::Int64Array>
         (tables_[nation],
          sNationkeyValue, n_nationkey, nationGetRowIdTime,
          nRegionkeyValue, n_regionkey, nationGetValTime,
          mapNum))
      continue;

    // n_regionkey = r_regionkey
    if (nRegionkeyValue != 3)
      continue;

    // add to revenue by nation key
    revenue[sNationkeyValue] += (1-lDiscountValue)*lExtendedpriceValue;
  }

  timer.Stop();
  std::stringstream ss;
  ss << " " << "Query 5 Chunk " << chunkNum ;
  ss << " " << "Rows = " << rowId << " Elapsed ms=" << timer.ElapsedInMicroseconds()/1000;
  ss << " " << "Orders RowId Time ms= " << ordersGetRowIdTime/1000;
  ss << " " << "Orders ValId Time ms= " << ordersGetValTime/1000;
  ss << " " << "Supplier RowId Time ms= " << supplierGetRowIdTime/1000;
  ss << " " << "Supplier ValId Time ms= " << supplierGetValTime/1000;
  ss << " " << "Nation RowId Time ms= " << nationGetRowIdTime/1000;
  ss << " " << "Nation ValId Time ms= " << nationGetValTime/1000;

  LOG(INFO) << ss.str() ;

}

std::map<std::string, double> TpchDemo::Query5()
{
  if (nullptr == tables_[lineitem] || nullptr == tables_[supplier] ||
      nullptr == tables_[orders] || nullptr == tables_[customer] ||
      nullptr == tables_[nation] || nullptr == tables_[region])
  {
    LOG(ERROR) <<  "No valid table to run Query5" ;
    return std::map<std::string, double>();
  }

  int64_t numChunks = lExtendedprice->num_chunks();

  // revenue for each chunk initialize
  ClearQ5Revenues();
  double **revenues;
  revenues = new double* [numChunks];
  for (int i=0; i<numChunks; i++)
  {
    revenues[i] = new double[25];
    for (int j=0; j<25; j++) {
      revenues[i][j] = 0;
    }
  }

  tbb::task_group tg;

  StopWatch timer;
  timer.Start();
  int64_t pnum=0;
  for (int64_t chunkNum = 0; chunkNum < numChunks; chunkNum++)
  {
    auto tf = std::bind(&TpchDemo::GetQuery5Revenue, this, chunkNum,
                        std::ref(revenues[chunkNum]), pnum%numMaps_);
    tg.run(tf);
    pnum++;
    if (pnum == numWorkers_)
    {
      pnum = 0;
      tg.wait();
    }
  }
  if (pnum > 0)
    tg.wait();

  for (int i=0; i<numChunks; i++)
  {
    for (int j=0; j<25; j++)
    {
      q5revenues[j] += revenues[i][j];
    }
  }

  timer.Stop();
  LOG(INFO) << "Query 5 Elapsed ms=" << timer.ElapsedInMicroseconds()/1000;
  auto result = GetAggrRevenues();
  return (move(result));

}

bool TpchDemo::MakeMaps()
{
  bool result = true;
  for (int32_t i=0; i<numTables; i++)
  {
    bool curResult = tables_[i]->MakeMaps(numMaps_);
    if (curResult)
    {
      LOG(INFO) << "Success " << tableNames[i];
      //tables_[i]->PrintMaps();
    }
    else
    {
      LOG(INFO) << "Fail " << tableNames[i];
    }
    result = result && curResult;
  }
  return result;
}

void TpchDemo::PrintSchemas()
{
  for (int32_t i=0; i<numTables; i++)
  {
    LOG(INFO) << "Table " << tableNames[i];
    tables_[i]->PrintSchema();
  }
}

void TpchDemo::PrintMaps()
{
  for (int32_t i=0; i<numTables; i++)
  {
    LOG(INFO) << "Table " << tableNames[i];
    tables_[i]->PrintMaps();
  }
}
