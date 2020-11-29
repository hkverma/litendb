#include <cstdint>
#include <iostream>
#include <vector>

#include "common.h"
#include "dtensor.h"

#include "TpchQ6Q5.h"
#include <tbb/tbb.h>

using namespace tendb;

void TpchQ6Q5::Query6Chunk(int64_t chunkNum, double& revenue)
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

}

double TpchQ6Q5::Query6()
{

  int64_t shipdateValue, quantityValue;
  double discountValue, extendedpriceValue;


  tbb::task_group tg;

  // for now do a full table scan need to build filtering metadata per column chunk
  lineitem_ = tCache_->GetTable("lineitem");
  if (!lineitem_)
  {
    LOG(ERROR) << "Cache does not have lineitem table" ;
    return 0;
  }
  auto arrTable = lineitem_->GetTable();  
  lShipdate = arrTable->column(l_shipdate);
  lDiscount = arrTable->column(l_discount);
  lQuantity = arrTable->column(l_quantity);
  lExtendedprice = arrTable->column(l_extendedprice);

  // constants
  date19970101Value =
    SecondsSinceEpoch(boost::gregorian::date(1997, 1, 1), boost::posix_time::seconds(0));
  date19971231Value =
    SecondsSinceEpoch(boost::gregorian::date(1997, 12, 31), boost::posix_time::seconds(0));  
  
  StopWatch timer;
  timer.Start();

  int64_t numChunks = lExtendedprice->num_chunks();
  std::vector<double> revenues(lExtendedprice->num_chunks());

  int64_t pnum=0;
  for (int64_t chunkNum = 0; chunkNum < numChunks; chunkNum++)
  {
    auto tf = std::bind(&TpchQ6Q5::Query6Chunk, this, chunkNum, std::ref(revenues[chunkNum]));
    tg.run(tf);
    pnum++;
    if (pnum == numParallels_)
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

double Tpch_Query6(TCache *tcache)
{
  // TODO Get rid of TpchQ6Q5 class
  std::shared_ptr<TCache> tcachePtr(tcache);
  auto tpchq6q5 = std::make_shared<TpchQ6Q5>(tcachePtr);
  double revenue = tpchq6q5->Query6();
  return revenue;
}
