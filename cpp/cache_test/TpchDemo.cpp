#include <TpchDemo.h>
#include <valgrind/callgrind.h>

namespace liten
{

// Tables - lineitem (first entry is fact table), rest are dimensions
const std::vector<std::string> TpchDemo::tableNames =
  {
    "lineitem",
    "customer",
    "orders",
    "supplier",
    "nation",
    "region"
  };

const std::vector<TableType> TpchDemo::tableTypes =
  {
    FactTable,
    DimensionTable,
    DimensionTable,
    DimensionTable,
    DimensionTable,
    DimensionTable
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
    DaysSinceEpoch(boost::gregorian::date(1997, 1, 1));
  date19971231Value =
    DaysSinceEpoch(boost::gregorian::date(1997, 12, 31));
  date19950101Value =
    DaysSinceEpoch(boost::gregorian::date(1995, 1, 1));
  date19951231Value =
    DaysSinceEpoch(boost::gregorian::date(1995, 12, 31));
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
    auto ttableResult  = std::move(tCache_->ReadCsv(tableNames[i], tableTypes[i], fileName,
                                                    readOptions, parseOptions, convertOptions));
    if (!ttableResult.ok())
    {
      TLOG(ERROR) << "Unable to Read file=" << fileName;
      continue;
    }
    tables_[i] = TCatalog::GetInstance()->GetTable(tableNames[i]);
    if (ttableResult.ValueOrDie() != tables_[i])
    {
      TLOG(ERROR) << "Error reading file=" << fileName;
      continue;
    }    
    tables_[i]->PrintTable(true, true);
  }

}

// Do these before running query, table may have changed
void TpchDemo::InitTpchTables()
{
  tables_.resize(numTables);
  for (int32_t i=0; i<numTables; i++)
  {
    auto ttable = TCatalog::GetInstance()->GetTable(tableNames[i]);
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
    lShipdate = tables_[lineitem]->GetColumn(l_shipdate);
    lDiscount = tables_[lineitem]->GetColumn(l_discount);
    lQuantity = tables_[lineitem]->GetColumn(l_quantity);
    lExtendedprice = tables_[lineitem]->GetColumn(l_extendedprice);
    lOrderkey = tables_[lineitem]->GetColumn(l_orderkey);
    lSuppkey = tables_[lineitem]->GetColumn(l_suppkey);
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
  TColumn::Iterator<int32_t, arrow::Int32Array> shipdateIter(lShipdate);
  TColumn::Iterator<double, arrow::DoubleArray> discountIter(lDiscount);
  TColumn::Iterator<int64_t, arrow::Int64Array> quantityIter(lQuantity);
  TColumn::Iterator<double, arrow::DoubleArray> extendedpriceIter(lExtendedprice);

  int64_t length = lShipdate->NumRows();
  if (length != lDiscount->NumRows() ||
      length != lQuantity->NumRows() ||
      length != lExtendedprice->NumRows())
  {
    LOG(ERROR) << "Length should be the same";
    return 0;
  }

  double revenue = 0;
  int32_t shipdateValue;
  int64_t quantityValue;
  double discountValue, extendedpriceValue;

  // for now do a full table scan need to build filtering metadata per column chunk
  for (int64_t rowId=0; rowId<length; rowId++)
  {
    if (!shipdateIter.Next(shipdateValue)) break;
    if (!discountIter.Next(discountValue)) break;
    if (!quantityIter.Next(quantityValue)) break;
    if (!extendedpriceIter.Next(extendedpriceValue)) break;
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
  auto shipdate = std::static_pointer_cast<arrow::Int32Array>(lShipdate->GetBlock(chunkNum)->GetArray());
  auto discount = std::static_pointer_cast<arrow::DoubleArray>(lDiscount->GetBlock(chunkNum)->GetArray());
  auto quantity = std::static_pointer_cast<arrow::Int64Array>(lQuantity->GetBlock(chunkNum)->GetArray());
  auto extendedprice = std::static_pointer_cast<arrow::DoubleArray>(lExtendedprice->GetBlock(chunkNum)->GetArray());
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

// Get Query6 Revenue
double TpchDemo::Query6()
{
  int32_t shipdateValue;
  int64_t quantityValue;
  double discountValue, extendedpriceValue;
  if (tables_[lineitem] == nullptr)
  {
    LOG(ERROR) <<  "No valid table to run Query6" ;
    return 0;
  }

  tbb::task_group tg;

  TStopWatch timer;
  timer.Start();

  // for now do a full table scan need to build filtering metadata per column chunk
  int64_t numChunks = lExtendedprice->NumBlocks();
  std::vector<double> revenues(lExtendedprice->NumBlocks());

  for (int64_t chunkNum = 0; chunkNum < numChunks; chunkNum++)
  {
    auto tf = std::bind(&TpchDemo::GetQuery6Revenue, this, chunkNum, std::ref(revenues[chunkNum]));
    tg.run(tf);
  }
  tg.wait();

  double revenue = 0;
  for (auto rev: revenues)
  {
    revenue += rev;
  }
  LOG(INFO) << "Completed Query6 Revenue=" << revenue;
  google::FlushLogFiles(google::INFO);
  return revenue;
}


inline void TpchDemo::ClearQ5Revenues()
{
  for (int i=0;i<numNations;i++)
  {
    q5revenues[i]=0;
  }
}

inline std::shared_ptr<std::unordered_map<std::string, double>> TpchDemo::GetAggrRevenues()
{
  std::shared_ptr<std::unordered_map<std::string, double>> result =
    std::make_shared<std::unordered_map<std::string, double>>();
  for (int i=0;i<numNations;i++)
  {
    if (q5revenues[i]>0) {
      result->insert(std::make_pair(nations[i], q5revenues[i]));
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

std::shared_ptr<std::unordered_map<std::string, double>> TpchDemo::Query5Serial()
{
  if (nullptr == tables_[lineitem] || nullptr == tables_[supplier] ||
      nullptr == tables_[orders] || nullptr == tables_[customer] ||
      nullptr == tables_[nation] || nullptr == tables_[region])
  {
    LOG(ERROR) <<  "No valid table to run Query5" ;
    return nullptr;
  }

  TColumn::Iterator<double, arrow::DoubleArray> lDiscountIter(lDiscount);
  TColumn::Iterator<double, arrow::DoubleArray> lExtendedpriceIter(lExtendedprice);
  TColumn::Iterator<int64_t, arrow::Int64Array> lOrderkeyIter(lOrderkey);
  TColumn::Iterator<int64_t, arrow::Int64Array> lSuppkeyIter(lSuppkey);

  int64_t length = lDiscount->NumRows();
  if (length != lExtendedprice->NumRows())
  {
    LOG(ERROR) << "Length should be the same";
    return nullptr;
  }

  ClearQ5Revenues();

  int64_t orderRowId;
  int32_t oOrderdateValue;
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


  TStopWatch timer;
  timer.Start();

  // for now do a full table scan need to build filtering metadata per column chunk
  TStopWatch totalTimer;
  totalTimer.Start();
  TStopWatch extraTimer;
  extraTimer.Start();
  TStopWatch scanTimer;
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
      extraTime += extraTimer.ElapsedInNanoseconds();
      countExtraTime = false;
    }

    scanTimer.Stop();
    scanTime += scanTimer.ElapsedInNanoseconds();

    if (rowId%rowIncrementsForTimeLog == 0)
    {
      totalTimer.Stop();
      totalTime += totalTimer.ElapsedInNanoseconds();
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
    if (!lOrderkeyIter.Next(lOrderkeyValue))
    {
      LOG(ERROR) << "Missing order key" ;
      break;
    }
    if (!lSuppkeyIter.Next(lSuppkeyValue))
    {
      LOG(ERROR) << "Missing supply key" ;
      break;
    }
    if (!lExtendedpriceIter.Next(lExtendedpriceValue))
    {
      LOG(ERROR) << "Missing extended price key" ;
      break;
    }
    if (!lDiscountIter.Next(lDiscountValue))
    {
      LOG(ERROR) << "Missing discount Value" ;
      break;
    }
    scanTimer.Stop();

    countExtraTime = true;
    extraTimer.Start();
    //
    // oOrderdateValue = lineitem_table[lOrderKeyValue][o_orderdate]
    // if (oOrderdateValue < date19950101Value || oOrderdateValue > date19951231Value)
    //   continue;
    //
    // sNationKeyValue = lineitem_table[lSuppkeyValue][[s_nationkey]
    // nRegionKeyValue = lineitem_table[sNationKeyValue][n_regionkey]
    //
    // if (nRegionKeyValue != 3)
    //  continue;
    //
    // l_orderkey = o_orderkey
    // and o_orderdate >= date '1995-01-01'
    // and o_orderdate < date '1995-01-01' + interval '1' year
    if (!tables_[orders]->JoinInner<int64_t, arrow::Int64Array, int32_t, arrow::Int32Array>
        (lOrderkeyValue, o_orderkey, ordersGetRowIdTime,
         oOrderdateValue, o_orderdate, ordersGetValTime))
      continue;
    if (oOrderdateValue < date19950101Value || oOrderdateValue > date19951231Value)
      continue;

    // Filter on r_name
    // l_suppkey = s_suppkey,
    if ( !tables_[supplier]->JoinInner<int64_t, arrow::Int64Array>
         (lSuppkeyValue, s_suppkey, supplierGetRowIdTime,
          sNationkeyValue, s_nationkey, supplierGetValTime))
      continue;

    // s_nationkey = n_nationkey also get nation name
    if ( !tables_[nation]->JoinInner<int64_t, arrow::Int64Array>
         (sNationkeyValue, n_nationkey, nationGetRowIdTime,
          nRegionkeyValue, n_regionkey, nationGetValTime))
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
  LOG(INFO) << "Query 5 Elapsed ms=" << timer.ElapsedInNanoseconds()/1000;
  auto q5result = GetAggrRevenues();
  return q5result;
}

void TpchDemo::GetQuery5Revenue(int64_t chunkNum, double revenue[])
{
  auto orderkey = std::static_pointer_cast<arrow::Int64Array>(lOrderkey->GetBlock(chunkNum)->GetArray());
  auto suppkey = std::static_pointer_cast<arrow::Int64Array>(lSuppkey->GetBlock(chunkNum)->GetArray());
  auto discount = std::static_pointer_cast<arrow::DoubleArray>(lDiscount->GetBlock(chunkNum)->GetArray());
  auto quantity = std::static_pointer_cast<arrow::Int64Array>(lQuantity->GetBlock(chunkNum)->GetArray());
  auto extendedprice = std::static_pointer_cast<arrow::DoubleArray>(lExtendedprice->GetBlock(chunkNum)->GetArray());
  TStopWatch timer;
  timer.Start();

  int64_t ordersGetRowIdTime=0, ordersGetValTime=0, extraTime=0, scanTime=0, totalTime=0;
  int64_t supplierGetRowIdTime=0, supplierGetValTime=0;
  int64_t nationGetRowIdTime=0, nationGetValTime=0;

  int64_t oOrderdateValue;
  int64_t sNationkeyValue, nRegionkeyValue;
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
    if (!tables_[orders]->JoinInner<int64_t, arrow::Int64Array, int64_t, arrow::Int64Array>
        (lOrderkeyValue, o_orderkey, ordersGetRowIdTime,
         oOrderdateValue, o_orderdate, ordersGetValTime))
      continue;
    //TLOG(INFO) << "Join " << rowId << "=" << oOrderdateValue;    
    if (oOrderdateValue < date19950101Value || oOrderdateValue > date19951231Value)
      continue;

    // Filter on r_name
    // l_suppkey = s_suppkey,
    if ( !tables_[supplier]->JoinInner<int64_t, arrow::Int64Array>
         (lSuppkeyValue, s_suppkey, supplierGetRowIdTime,
          sNationkeyValue, s_nationkey, supplierGetValTime))
      continue;

    // s_nationkey = n_nationkey also get nation name
    if ( !tables_[nation]->JoinInner<int64_t, arrow::Int64Array>
         (sNationkeyValue, n_nationkey, nationGetRowIdTime,
          nRegionkeyValue, n_regionkey, nationGetValTime))
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
  ss << " " << "Rows = " << rowId << " Elapsed ms=" << timer.ElapsedInNanoseconds()/1000;
  ss << " " << "Orders RowId Time ns= " << ordersGetRowIdTime;
  ss << " " << "Orders ValId Time ns= " << ordersGetValTime;
  ss << " " << "Supplier RowId Time ns= " << supplierGetRowIdTime;
  ss << " " << "Supplier ValId Time ns= " << supplierGetValTime;
  ss << " " << "Nation RowId Time ns= " << nationGetRowIdTime;
  ss << " " << "Nation ValId Time ns= " << nationGetValTime;

  TLOG(INFO) << ss.str() ;

}

void TpchDemo::GetQuery5RevenueTensor(int64_t chunkNum, double revenue[])
{
  TStopWatch loopTimer, lkupTimer, exprTimer, spotTimer;
  
  int64_t ordersTime=0, regionTime=0, nationTime=0;
  int64_t exprTime=0, lkupTime=0, totalTime=0;

  int32_t oOrderdateValue;
  int64_t sNationkeyValue, nRegionkeyValue;

  TRowId rowId;
  TRowId parentRowId;
  int64_t filteredRows=0;
  
  rowId.blkNum=chunkNum;
  TLOG(INFO) << "Query5 Processing block=" << rowId.blkNum;
  
  int64_t blkLength = tables_[lineitem]->GetColumn(l_orderkey)->GetBlock(chunkNum)->GetArray()->length();

  for (rowId.rowNum=0; rowId.rowNum<blkLength; rowId.rowNum++)
  {
    loopTimer.Start();

    auto lkup =[&]()
      { // Lookup block
          
        // l_orderkey = o_orderkey
        // and o_orderdate >= date '1995-01-01'
        // and o_orderdate < date '1995-01-01' + interval '1' year
        spotTimer.Start();
        auto oOrderdateValueResult =
        std::move(
                  tables_[lineitem]->GetValue<int64_t, arrow::Int64Array>
                  (rowId,       // lineitem rowId
                   l_orderkey,  // column number where orderkey is present
                   o_orderdate, // order date column number
                   parentRowId) // parent row id
                  );
        spotTimer.Stop();
        ordersTime += spotTimer.ElapsedInNanoseconds();
      
        if (!oOrderdateValueResult.ok())
        {
          TLOG(ERROR) << "Failed to get orderdate msg=" << oOrderdateValueResult.status().message();
          return;
        }
        oOrderdateValue = oOrderdateValueResult.ValueOrDie();  // order date value return
        //TLOG(INFO) << "Tensor " << rowId << "=" << oOrderdateValue;
        if (oOrderdateValue < date19950101Value || oOrderdateValue > date19951231Value)
          return;

        // Filter on r_name
        // l_suppkey = s_suppkey
        spotTimer.Start();
        auto sNationkeyValueResult =
        std::move(
                  tables_[lineitem]->GetValue<int64_t, arrow::Int64Array>
                  (rowId,         // lineitem row Id
                   l_suppkey,     // supply key column number
                   s_nationkey,   // supplier nation key column
                   parentRowId)   // parent row id
                  );
        spotTimer.Stop();
        nationTime += spotTimer.ElapsedInNanoseconds();
      
        if (!sNationkeyValueResult.ok())
        {
          TLOG(ERROR) << "Failed to get nationkey msg=" << sNationkeyValueResult.status().message() << " Blk Num=" << rowId.blkNum << " Row Num=" << rowId.rowNum << " lineitem RowId=" << tables_[lineitem]->GetColumn(l_suppkey)->GetRowNum(rowId) << " supplier RowId=" << tables_[supplier]->GetColumn(s_nationkey)->GetRowNum(parentRowId);
          return;
        }
        sNationkeyValue = sNationkeyValueResult.ValueOrDie();
        spotTimer.Start();
        auto nRegionkeyValueResult =
        std::move(
                  tables_[supplier]->GetValue<int64_t, arrow::Int64Array>
                  (parentRowId,   // lineitem row Id
                   s_nationkey,   // supplier nation key column
                   n_regionkey,   // get region from nation
                   parentRowId)   // parent row id         
                  );
        spotTimer.Stop();
        regionTime += spotTimer.ElapsedInNanoseconds();
          
        if (!nRegionkeyValueResult.ok())
        {
          TLOG(ERROR) << "Failed to get orderdate msg=" << nRegionkeyValueResult.status().message();
          return;
        }
        nRegionkeyValue = nRegionkeyValueResult.ValueOrDie();  // nationkey from supplier table
      };

      
    lkupTimer.Start();
    lkup();
    lkupTimer.Stop();
    lkupTime += lkupTimer.ElapsedInNanoseconds();

    auto aggr = [&]() 
      { // expr measurement blocks

        // n_regionkey = r_regionkey
        if (nRegionkeyValue != 3)
          return;
    
        // add to revenue by nation key      
        // Get all values from lineitem revenue calculation
        auto lExtendedpriceValueResult =
        std::move(tables_[lineitem]->GetColumn(l_extendedprice)->GetValue<double, arrow::DoubleArray>(rowId));
        if (!lExtendedpriceValueResult.ok())
        {
          TLOG(ERROR) << "Invalid extended price value" ;
          return;
        }
        auto lExtendedpriceValue = lExtendedpriceValueResult.ValueOrDie();
      
        auto lDiscountValueResult =
        std::move(tables_[lineitem]->GetColumn(l_discount)->GetValue<double, arrow::DoubleArray>(rowId));
        if (!lDiscountValueResult.ok())
        {
          TLOG(ERROR) << "Invalid extended price value" ;
          return;
        }
        auto lDiscountValue = lDiscountValueResult.ValueOrDie();
    
        filteredRows++;
        revenue[sNationkeyValue] += (1-lDiscountValue)*lExtendedpriceValue;
      };

    exprTimer.Start();
    aggr();
    exprTimer.Stop();
    exprTime += exprTimer.ElapsedInNanoseconds();

    loopTimer.Stop();
    totalTime += loopTimer.ElapsedInNanoseconds();
  }
  
  std::stringstream ss;
  ss << " " << "Query 5  Blk " << rowId.blkNum ;
  ss << " " << "Total Rows = " << rowId.rowNum << " Filtered rows=" << filteredRows;
  ss << " " << "Elapsed ns=" << totalTime;
  ss << " " << "Total lookup time ns=" << lkupTime;
  ss << " " << "Expr eval time ns=" << exprTime;
  ss << " " << "Orders Time ns= " << ordersTime;
  ss << " " << "Nation Time ns= " << nationTime;
  ss << " " << "Region Time ns= " << regionTime;

  TLOG(INFO) << ss.str() ;

}

std::shared_ptr<std::unordered_map<std::string, double>> TpchDemo::Query5(bool useTensor)
{
  if (nullptr == tables_[lineitem] || nullptr == tables_[supplier] ||
      nullptr == tables_[orders] || nullptr == tables_[customer] ||
      nullptr == tables_[nation] || nullptr == tables_[region])
  {
    TLOG(ERROR) <<  "No valid table to run Query5" ;
    return nullptr;
  }

  int64_t numChunks = lExtendedprice->NumBlocks();

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

  auto taskScheduler = TTaskScheduler::GetInstance();
  int64_t numaId = 0;

  CALLGRIND_START_INSTRUMENTATION;
  CALLGRIND_TOGGLE_COLLECT;

  TStopWatch timer;
  timer.Start();
  for (int64_t chunkNum = 0; chunkNum < numChunks; chunkNum++)
  {
    if (useTensor)
    {
      auto tf = std::bind(&TpchDemo::GetQuery5RevenueTensor, this, chunkNum,
                     std::ref(revenues[chunkNum]));
      //taskScheduler->Execute(tf, numaId);
      tf();
    }
    else
    {
      auto tf = std::bind(&TpchDemo::GetQuery5Revenue, this, chunkNum,
                          std::ref(revenues[chunkNum]));
      taskScheduler->Execute(tf, numaId);
    }
  }
  taskScheduler->Wait(numaId);
  
  for (int i=0; i<numChunks; i++)
  {
    for (int j=0; j<25; j++)
    {
      q5revenues[j] += revenues[i][j];
    }
  }

  timer.Stop();
  CALLGRIND_TOGGLE_COLLECT;
  CALLGRIND_STOP_INSTRUMENTATION;
  
  LOG(INFO) << "Query 5 Elapsed ms=" << timer.ElapsedInNanoseconds();
  auto result = GetAggrRevenues();
  google::FlushLogFiles(google::INFO);
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

void TpchDemo::PrintTables()
{
  for (int32_t i=0; i<numTables; i++)
  {
    PrintTable(i);
  }
}

void TpchDemo::PrintTable(int num)
{
  TLOG(INFO) << "Table " << tableNames[num];
  tables_[num]->PrintTable(true, true);
}

}
