#include <TTable.h>
#include <TColumn.h>
#include <TColumnMap.h>
#include <TCatalog.h>
#include <TBlock.h>
#include <TRowBlock.h>
#include <TSchema.h>

namespace liten
{

std::vector<std::string> TableTypeString = {"dim","fact"};

// Create a new TTable
TResult<std::shared_ptr<TTable>> TTable::Create(std::string tableName,
                                                TableType type,
                                                std::shared_ptr<arrow::Table> table)
{
  auto ttable = TCatalog::GetInstance()->GetTable(tableName);
  if (nullptr != ttable)
  {
    return TResult<std::shared_ptr<TTable>>(TStatus::AlreadyExists("Table=",tableName," is already in catalog"));
  }
  std::string schemaName = tableName+"_schema";
  auto schema = TSchema::Create(table->schema(), type, schemaName);
  if (!schema.ok())
  {
    return TResult<std::shared_ptr<TTable>>(TStatus::AlreadyExists("Table=",tableName," could not be created because schema=", schemaName, "failed to create with msg=", schema.status().message()));
  }
  ttable = std::make_shared<MakeSharedEnabler>();
  ttable->schema_ = schema.ValueOrDie();
  ttable->name_ = std::move(tableName);
  ttable->table_ = table;
  TStatus status = std::move(ttable->AddToCatalog());
  if (!status.ok())
  {
    return TResult<std::shared_ptr<TTable>>(status);
  }
  return TResult<std::shared_ptr<TTable>>(ttable);
}

// Add all blocks to catalog
// TBD create TRowBlock
// TBD remove all the Arrow pointers once the blocks are created
// TBD maintain a local vector of rowblocks as well
// TBD move addToCatalog to Rowblock and use that instead
TStatus TTable::AddToCatalog() {
  const std::vector<std::string>& colNames = schema_->GetSchema()->field_names();
  assert(colNames.size() == table_->num_columns());
  for (auto colNum = 0; colNum>table_->num_columns(); colNum++)
  {
    auto col = std::make_shared<TColumn>(colNames[colNum], type_, table_->column(colNum));
    TStatus status = std::move(col->AddToCatalog());
    if (!status.ok()) {
      return status;
    }
  }
  // Create a rowblock now
  const std::vector<std::shared_ptr<arrow::ChunkedArray>>& chunkedArrays = table_-> columns();
  // Nothing to build
  if (0 == chunkedArrays.size())
  {
    return TStatus::OK();
  }
  auto numChunks = chunkedArrays[0]->num_chunks();
  for (auto chunkNum=0; chunkNum<numChunks; chunkNum++)
  {
    std::vector<std::shared_ptr<TBlock>> columns;
    auto numRows = chunkedArrays[0]->chunk(chunkNum)->length();
    for (auto colNum = 0; colNum>table_->num_columns(); colNum++)
    {
      auto arr = chunkedArrays[colNum]->chunk(chunkNum);
      assert(numRows == arr->length());
      auto blk = TBlock::Create(arr);
      if (nullptr == blk)
      {
        return TStatus::UnknownError("Cannor create a block.");
      }
      columns.push_back(blk);
    }
    auto trb = TRowBlock::Create(type_, schema_, numRows, columns);
    rowBlocks_.push_back(trb);
  }
  TStatus status = TCatalog::GetInstance()->AddTable(shared_from_this());
  return std::move(status);
}
  
// Print Schema in logfile
void TTable::PrintSchema()
{
  const std::vector<std::shared_ptr<arrow::Field>>& tableSchemaFields = schema_->GetSchema()->fields();
  std::stringstream ss;
  ss << "Schema=";

  for (auto schemaField : tableSchemaFields) 
  {
    ss << "{" << schemaField->ToString() << "}," ;
  }
  TLOG(INFO) << ss.str();
}

// print table in logfile
void TTable::PrintTable()
{
  std::stringstream ss;
  ss << "NumCols=" << NumColumns();
  ss << " NumRows=" << NumRows() << " Data=";

  // Print the table
  for (int64_t i=0; i<NumColumns(); i++)
  {
    auto chunkedArray = table_->column(i);
    const std::shared_ptr<arrow::Field>& colField = schema_->GetSchema()->field(i);
    const std::shared_ptr<arrow::DataType>& colFieldType = colField->type();
       
    for (int64_t j=0; j<chunkedArray->num_chunks(); j++)
    {
      auto aray = chunkedArray->chunk(j);
      for (int64_t k=0; k<aray->length(); k++)
      {
        arrow::Result<std::shared_ptr<arrow::Scalar>> dataResult = aray->GetScalar(k);
        if ( !dataResult.ok() )
        {
          ss << ",";
        }
        else
        {
          std::shared_ptr<arrow::Scalar> data = dataResult.ValueOrDie();
          if (data->is_valid)
            ss << data->ToString();
          ss << ",";
        }
      }
    }
  }
  TLOG(INFO) << ss.str();
  TLog::GetInstance()->FlushLogFiles(google::INFO);
}

// Returns non-zero code if fails to make map
int TTable::MakeMaps(int32_t numCopies)
{
  if (nullptr == table_)
  {
    return 1;
  }
  if (numCopies < 1)
  {
    return 1;
  }
  numMapCopies_ = numCopies;
  maps_.resize(numCopies);
  for (auto nc = 0; nc < numCopies; nc++)
  {
    maps_[nc].resize(table_->num_columns());
  }    
  for (int64_t cnum=0; cnum<table_->num_columns(); cnum++)
  {
    std::shared_ptr<arrow::ChunkedArray> chunkedArray = table_->column(cnum);
    maps_[0][cnum] = TColumnMap::Make(chunkedArray);
  }
  for (auto nc = 1; nc<numCopies; nc++)
  {
    for (auto cnum=0; cnum<table_->num_columns(); cnum++)
    {
      maps_[nc][cnum] = maps_[0][cnum]->Copy();
    }
  }
  return 0;
}

void TTable::PrintMaps()
{
  std::stringstream ss;
  for (int colNum = 0; colNum < table_->num_columns(); colNum++) {
    auto colMap = maps_[0][colNum];
    ss << "Col " << colNum;
    auto chArr = colMap->chunkedArray_;
    for (int arrNum = 0; arrNum<chArr->num_chunks(); arrNum++)
    {
      int64_t minVal, maxVal;
      auto arr = chArr->chunk(arrNum);
      ss << " Arr " << arrNum << " Size=" << arr->length();
      ss << " Type=" << arr->type()->ToString() ;
      ss << " Min=";
      colMap->GetMin(arrNum,minVal)?(ss << minVal):(ss << "None");
      ss << " Max=";
      colMap->GetMax(arrNum,maxVal)?(ss << maxVal):(ss << "None");
      ss << ";" ;
    }
    //colMap->GetReverseMap(ss);
    //ss << "; ";
  }
  TLOG(INFO) << ss.str();
}

// This gives a slice from offset from beginning of length length
std::shared_ptr<arrow::Table> TTable::Slice(int64_t offset, int64_t length) {
  auto arrTable = table_->Slice(offset, length);
  return arrTable;
}

}
