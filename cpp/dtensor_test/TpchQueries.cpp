#include <cstdint>
#include <iostream>
#include <vector>

#include "common.h"
#include "dtensor.h"

#include "TpchQueries.h"


using namespace tendb;

void TpchQueries::ReadTables()
{
  arrow::csv::ReadOptions readOptions = arrow::csv::ReadOptions::Defaults();
  arrow::csv::ParseOptions parseOptions = arrow::csv::ParseOptions::Defaults();
  parseOptions.delimiter = '|';
  arrow::csv::ConvertOptions convertOptions = arrow::csv::ConvertOptions::Defaults();

  tables_.resize(numTables);

  for (int32_t i=0; i<numTables; i++)
  {
    std::string fileName = tpchDir_ + tableNames[i] + ".tbl";
    tables_[i] = tCache_->ReadCsv(tableNames[i], fileName,
                                  readOptions, parseOptions, convertOptions);
    //tables_[i]->Print();
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
double TpchQueries::Query6()
{
  std::shared_ptr<arrow::ChunkedArray> shipdate = tables_[lineitem]->table_->column(l_shipdate);
  std::shared_ptr<arrow::ChunkedArray> discount = tables_[lineitem]->table_->column(l_discount);
  std::shared_ptr<arrow::ChunkedArray> quantity = tables_[lineitem]->table_->column(l_quantity);
  std::shared_ptr<arrow::ChunkedArray> extendedprice = tables_[lineitem]->table_->column(l_extendedprice);

  int shipdateChunkNum=0, discountChunkNum=0, quantityChunkNum=0, extendedpriceChunkNum=0;
  TColumnIterator<int64_t, arrow::Int64Array> shipdateIter(shipdate);
  TColumnIterator<double, arrow::DoubleArray> discountIter(discount);
  TColumnIterator<int64_t, arrow::Int64Array> quantityIter(quantity);
  TColumnIterator<double, arrow::DoubleArray> extendedpriceIter(extendedprice);

  int64_t length = shipdate->length();
  if (length != discount->length() ||
      length != quantity->length() ||
      length != extendedprice->length())
  {
    std::cout << "Length should be the same" << std::endl;
    return 0;
  }

  double revenue = 0;
  int64_t shipdateValue, quantityValue;
  double discountValue, extendedpriceValue;

  int64_t date19970101Value =
    SecondsSinceEpoch(boost::gregorian::date(1997, 1, 1), boost::posix_time::seconds(0));
  int64_t date19971231Value =
    SecondsSinceEpoch(boost::gregorian::date(1997, 12, 31), boost::posix_time::seconds(0));

  StopWatch timer;
  timer.Start();
  // for now do a full table scan need to build filtering metadata per column chunk
  for (int64_t rowId=0; rowId<length; rowId++)
  {
    if (rowId%10000 == 0) {
      timer.Stop();
      std::cout << "Rows = " << rowId << " Elapsed ms=" << timer.ElapsedInMilliseconds() << std::endl;
    }
    
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

double TpchQueries::Query5()
{

  std::shared_ptr<arrow::ChunkedArray> lDiscount =
    tables_[lineitem]->table_->column(l_discount);
  std::shared_ptr<arrow::ChunkedArray> lExtendedprice =
    tables_[lineitem]->table_->column(l_extendedprice);
  std::shared_ptr<arrow::ChunkedArray> lOrderkey =
    tables_[lineitem]->table_->column(l_orderkey);
  std::shared_ptr<arrow::ChunkedArray> lSuppkey =
    tables_[lineitem]->table_->column(l_suppkey);


  std::shared_ptr<arrow::ChunkedArray> oOrderkey =
    tables_[orders]->table_->column(o_orderkey);
  std::shared_ptr<arrow::ChunkedArray> oOrderdate =
    tables_[orders]->table_->column(o_orderdate);

  std::shared_ptr<arrow::ChunkedArray> sSuppkey =
    tables_[supplier]->table_->column(s_suppkey);
  std::shared_ptr<arrow::ChunkedArray>  sNationkey =
    tables_[supplier]->table_->column(s_nationkey);

  std::shared_ptr<arrow::ChunkedArray>  nNationkey =
    tables_[nation]->table_->column(n_nationkey);
  std::shared_ptr<arrow::ChunkedArray>  nRegionkey =
    tables_[nation]->table_->column(n_regionkey);

  std::shared_ptr<arrow::ChunkedArray>  rRegionkey =
    tables_[region]->table_->column(r_regionkey);
  std::shared_ptr<arrow::ChunkedArray>  rName =
    tables_[region]->table_->column(r_name);

  TColumnIterator<double, arrow::DoubleArray> lDiscountIter(lDiscount);
  TColumnIterator<double, arrow::DoubleArray> lExtendedpriceIter(lExtendedprice);
  TColumnIterator<int64_t, arrow::Int64Array> lOrderkeyIter(lOrderkey);
  TColumnIterator<int64_t, arrow::Int64Array> lSuppkeyIter(lSuppkey);

  int64_t length = lDiscount->length();
  if (length != lExtendedprice->length())
  {
    std::cout << "Length should be the same" << std::endl;
    return 0;
  }

  int64_t orderRowId, oOrderdateValue;
  int64_t suppRowId, sNationkeyValue;
  int64_t nationRowId;
  int64_t nRegionkeyValue;
  int64_t regionRowId;
  //arrow::util::string_view rNameValue;
  std::string rNameValue;
  int64_t lOrderkeyValue, lSuppkeyValue;
  double lDiscountValue, lExtendedpriceValue;

  double revenue = 0;

  int64_t date19950101Value =
    SecondsSinceEpoch(boost::gregorian::date(1995, 1, 1), boost::posix_time::seconds(0));
  int64_t date19951231Value =
    SecondsSinceEpoch(boost::gregorian::date(1995, 12, 31), boost::posix_time::seconds(0));
  std::string europe("EUROPE");
  std::string rNationValue;

  // for now do a full table scan need to build filtering metadata per column chunk
  StopWatch timer;
  timer.Start();
  for (int64_t rowId=0; rowId<length; rowId++)
  {
    if (rowId%10000 == 0) {
      timer.Stop();
      std::cout << "Rows = " << rowId << " Elapsed ms=" << timer.ElapsedInMilliseconds() << std::endl;
    }
    // Get all values for the row first
    if (!lOrderkeyIter.next(lOrderkeyValue)) break;
    if (!lSuppkeyIter.next(lSuppkeyValue)) break;
    if (!lExtendedpriceIter.next(lExtendedpriceValue)) break;
    if (!lDiscountIter.next(lDiscountValue)) break;

    // Filter on orderdata
    if (!GetRowId<int64_t, arrow::Int64Array>(orderRowId, lOrderkeyValue, tables_[orders], o_orderkey)) continue;
    if (!GetValue<int64_t, arrow::Int64Array>(orderRowId, oOrderdateValue, oOrderdate)) continue;
    if (oOrderdateValue < date19950101Value || oOrderdateValue > date19951231Value)
      continue;

    // Filter on r_name
    // l_suppkey = s_suppkey, s_nationkey = n_nationkey, n_regionkey = r_regionkey
    //   r_name = 'EUROPE'
    if (!GetRowId<int64_t, arrow::Int64Array>(suppRowId, lSuppkeyValue, tables_[supplier], s_suppkey)) continue;
    if (!GetValue<int64_t, arrow::Int64Array>(suppRowId, sNationkeyValue, sNationkey)) continue;

    if (!(GetRowId<int64_t, arrow::Int64Array>(nationRowId, sNationkeyValue, tables_[nation], n_nationkey))) continue;
    if (!(GetValue<int64_t, arrow::Int64Array>(nationRowId, nRegionkeyValue, nRegionkey))) continue;

    if (!(GetRowId<int64_t, arrow::Int64Array>(regionRowId, nRegionkeyValue, tables_[region], r_regionkey))) continue;
    //if (!(GetValue<arrow::util::string_view, arrow::StringArray>(regionRowId, rNameValue, rName))) continue;
    if (!(GetValue<std::string, arrow::StringArray>(regionRowId, rNameValue, rName))) continue;

    bool ifEurope = std::equal(europe.begin(), europe.end(), rNameValue.begin(),
                               [] (const char& a, const char& b)
                               {
                                 return (std::tolower(a) == std::tolower(b));
                               });
    if (!ifEurope)
    continue;

    // add to revenue
    revenue += (1-lDiscountValue)*lExtendedpriceValue;
  }
  return revenue;
}


bool TpchQueries::MakeMaps()
{
  bool result = true;
  for (int32_t i=0; i<numTables; i++)
  {
    bool curResult = tables_[i]->MakeMaps();
    if (curResult)
    {
      std::cout << "Success " << tableNames[i] << std::endl;
      tables_[i]->PrintMaps();
    }
    else
    {
      std::cout << "Fail " << tableNames[i] << std::endl;      
    }
    result = result && curResult;
  }
  return result;
}

void TpchQueries::PrintSchemas()
{
  for (int32_t i=0; i<numTables; i++)
  {
    std::cout << "Table " << tableNames[i] << std::endl;
    tables_[i]->PrintSchema();
  }
}

void TpchQueries::PrintMaps()
{
  for (int32_t i=0; i<numTables; i++)
  {
    std::cout << "Table " << tableNames[i] << std::endl;
    tables_[i]->PrintMaps();
  }
}
