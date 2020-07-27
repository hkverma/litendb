
#include <cstdint>
#include <iostream>
#include <vector>

#include <arrow/api.h>
#include <arrow/csv/api.h>
#include <arrow/filesystem/api.h>
#include <arrow/io/api.h>

#include <glog/logging.h>
#include "dtensor.h"

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace tendb;

//
// TODO
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

#define EXIT_ON_FAILURE(expr)                      \
  do {                                             \
    arrow::Status status_ = (expr);                \
    if (!status_.ok()) {                           \
      std::cerr << status_.message() << std::endl; \
      return EXIT_FAILURE;                         \
    }                                              \
  } while (0);


template <class Type, class TypeArray>
class ColumnValueIterator
{
public:
  ColumnValueIterator(std::shared_ptr<arrow::ChunkedArray> chary) :
    currentArrayRowId_(0), lastArrayRowId_(0), chunkNum_(0)
  {
    chunkedArray_ = chary;
    array_ =
      std::static_pointer_cast<TypeArray>(chunkedArray_->chunk(chunkNum_));
    chunkNum_++;
  }

  bool next(Type& data)
  {
    int64_t rowId = currentArrayRowId_+lastArrayRowId_;
    if (rowId >= chunkedArray_->length())
      return false;

    while (currentArrayRowId_ >= array_->length())
    {
      if (!nextArray())
      {
        return false;
      }
      lastArrayRowId_ += currentArrayRowId_;
      currentArrayRowId_ = 0;
    }
    data = array_->Value(currentArrayRowId_);
    currentArrayRowId_++;
    return true;
  }

  bool nextArray()
  {
    chunkNum_++;
    if (chunkNum_ >= chunkedArray_->num_chunks())
    {
      return false;
    }
    array_ =
      std::static_pointer_cast<TypeArray>(chunkedArray_->chunk(chunkNum_));
    return true;
  }

  int64_t currentArrayRowId_;
  int64_t lastArrayRowId_;
  int64_t chunkNum_;
  std::shared_ptr<TypeArray> array_;
  std::shared_ptr<arrow::ChunkedArray> chunkedArray_;
};

// using namespace boost::posix_time;
// Use time_point calendars in C++-20 standards
int64_t SecondsSinceEpoch(boost::gregorian::date d, boost::posix_time::time_duration td)
{
  boost::posix_time::ptime t(d, td);
  boost::posix_time::ptime start(boost::gregorian::date(1970,1,1));
  boost::posix_time::time_duration dur = t - start;
  time_t epoch = dur.total_seconds();
  return (int64_t)epoch;
}


// ISO 86101 string "2002-01-20 23:59:59"
int64_t SecondsSinceEpoch(std::string& ts)
{
  boost::posix_time::ptime t(boost::posix_time::time_from_string(ts));
  boost::posix_time::ptime start(boost::gregorian::date(1970,1,1));
  boost::posix_time::time_duration dur = t - start;
  time_t epoch = dur.total_seconds();
  return (int64_t)epoch;
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
double Query6(std::shared_ptr<TTable> ttable)
{

  int l_shipdate=10, l_discount=6, l_quantity=4, l_extendedprice=5;

  std::shared_ptr<arrow::ChunkedArray> shipdate = ttable->table_->column(l_shipdate);
  std::shared_ptr<arrow::ChunkedArray> discount = ttable->table_->column(l_discount);
  std::shared_ptr<arrow::ChunkedArray> quantity = ttable->table_->column(l_quantity);
  std::shared_ptr<arrow::ChunkedArray> extendedprice = ttable->table_->column(l_extendedprice);

  int shipdateChunkNum=0, discountChunkNum=0, quantityChunkNum=0, extendedpriceChunkNum=0;
  ColumnValueIterator<int64_t, arrow::Int64Array> shipdateIter(shipdate);
  ColumnValueIterator<double, arrow::DoubleArray> discountIter(discount);
  ColumnValueIterator<int64_t, arrow::Int64Array> quantityIter(quantity);
  ColumnValueIterator<double, arrow::DoubleArray> extendedpriceIter(extendedprice);

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

  double result = Query6(ttable);
  std::cout << "Revenue=" << result << std::endl;

  return EXIT_SUCCESS;

}
