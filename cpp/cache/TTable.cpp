#include <TColumnMap.h>
#include <TCatalog.h>
#include <TBlock.h>
#include <TRowBlock.h>
#include <TSchema.h>
#include <TColumn.h>
#include <TTable.h>

namespace liten
{

std::vector<std::string> TableTypeString = {"DimensionTable","FactTable"};

// Create a new TTable
TResult<std::shared_ptr<TTable>> TTable::Create(std::string tableName,
                                                TableType type,
                                                std::string schemaName)
{
  if (schemaName.empty()) {
    schemaName = tableName+"_schema";
  }
  auto ttable = std::make_shared<MakeSharedEnabler>();
  ttable->name_ = std::move(tableName);
  ttable->schema_ = nullptr;
  ttable->type_ = type;
  return ttable;
}

// Add all blocks to catalog
// TBD create TRowBlock
// TBD remove all the Arrow pointers once the blocks are created
// TBD maintain a local vector of rowblocks as well
// TBD move addToCatalog to Rowblock and use that instead
/* TODO
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
*/  
// Print Schema in logfile
void TTable::PrintSchema()
{
  std::string schStr = std::move(schema_->ToString());
  TLOG(INFO) << schStr;
}

// print table in logfile
void TTable::PrintTable()
{
  std::stringstream ss;
  ss << "NumCols=" << NumColumns();
  ss << " NumRows=" << NumRows() << " Data=";

  // Print the table
  for (int64_t i=0; i<NumRowBlocks(); i++)
  {
    auto rb = GetRowBlock(i);
    for (int64_t j=0; i<NumColumns(); i++)
    {
      auto numRows = rb->NumRows();
      
      auto tblk = rb->GetBlock(j);
      if (!tblk)
      {
        ss << "Error Getting block for rowblock";
        continue;
      }
      auto arr = tblk->GetArray();
      
      assert (numRows <= arr->length());
      for (int64_t k=0; k<numRows; k++)
      {
        arrow::Result<std::shared_ptr<arrow::Scalar>> dataResult = arr->GetScalar(k);
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
// TBD move to TColumn
int TTable::MakeMaps()
{
  for (int64_t cnum=0; cnum<NumColumns(); cnum++)
  {
    std::shared_ptr<TColumn> col = GetColumn(cnum);
    auto colResult = TColumnMap::Create(col);
    if (!colResult.ok())
    {
      TLOG(ERROR) << "Could not create maps" ;
      return -1;
    }
  }
  return 0;
}

void TTable::PrintMaps()
{
  std::stringstream ss;
  for (int colNum = 0; colNum < NumColumns(); colNum++) {
    /*    auto colMap = maps_[colNum];
    ss << "Col " << colNum;
     TBD Do proper printing move to TColumn
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
      }*/
    //colMap->GetReverseMap(ss);
    //ss << "; ";
  }
  TLOG(INFO) << ss.str();
}

/// Append the rowblock to the table
TResult<std::shared_ptr<TRowBlock>> TTable::AppendRowBlock(std::shared_ptr<arrow::RecordBatch> arrb,
                                                           int64_t numRows)
{
  // Create rowblock
  auto rb_result = std::move(TRowBlock::Create(shared_from_this(), arrb, numRows));
  if (!rb_result.ok())
  {
    return std::move(rb_result);
  }
  auto rb = rb_result.ValueOrDie();
  if (rb->NumColumns() != columns_.size())
  {
    return TStatus::Invalid("Different number of columns in rowblock=", rb->NumColumns(),
                            "and table columns=", columns_.size(), " in table ", name_);
  }

  // Check and create schema
  if (nullptr == schema_)
  {
    auto tschResult = AddSchema(arrb->schema(), schema_->GetName());
    if (!tschResult.ok())
    {
      return TStatus::KeyError("Table=",name_," could not be created because schema=", schema_->GetName(), " failed to create with msg=", tschResult.status().ToString());
    }
  }
  else
  {
    if (schema_->GetSchema() != arrb->schema())
    {
      return TStatus::KeyError("Table=",name_," could not be created because schema name=", schema_->GetName(), " has different arrow schema than given table's schema");
    }
  }

  // Now add all blocks to the columns
  for (auto i=0; i<rb->NumColumns(); i++)
  {
    auto tblk = rb->GetBlock(i);
    if (!tblk) {
      return TStatus::Invalid("GetBlock was not valid");
    }
    auto status = columns_[i]->Add(tblk);
    if (!status.ok())
        return status;
  }

  // Now add the rowblocks
  rowBlocks_.push_back(rb);
  numRows += rb->NumRows();
  
  return rb_result;
}

// Add Schema to the table
TResult<std::shared_ptr<TSchema>> TTable::AddSchema(std::shared_ptr<arrow::Schema> schema, std::string schemaName)
{
  if (schemaName.empty()) {
    schemaName = name_+"_schema";
  }
  std::shared_ptr<TSchema> tschema = TCatalog::GetInstance()->GetSchema(schemaName);
  if (nullptr == tschema)
  {
    auto tschemaResult = TSchema::Create(schemaName, type_, schema);
    if (!tschemaResult.ok())
    {
      return TStatus::AlreadyExists("In Table=",name_," schema=", schema_->GetName(), " failed to create with msg=", tschemaResult.status().message());
    }
    tschema = tschemaResult.ValueOrDie();
  }
  schema_ = tschema;
  return schema_;
}

std::shared_ptr<TBlock> TTable::GetBlock(int64_t rbNum, int64_t colNum)
{
  auto rblk = GetRowBlock(rbNum);
  if (!rblk) return nullptr;
  auto blk = rblk->GetBlock(colNum);
  return blk;
}

}
