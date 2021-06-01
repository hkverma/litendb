#include <iostream>
#include <sstream>
#include <arrow/csv/api.h>
#include <arrow/filesystem/api.h>
#include <arrow/io/api.h>

#include <common.h>
#include "TTable.h"


using namespace liten;

TTable::TTable(std::string name, std::shared_ptr<arrow::Table> table, TType type)
  : table_(table), type_(type)
{
  schema_ = table_->schema();
  name_ = move(name);
}
  
void TTable::PrintSchema()
{
  // Print Table for now
  const std::vector<std::shared_ptr<arrow::Field>>& tableSchemaFields = schema_->fields();
  std::stringstream ss;
  ss << "Schema=";

  for (auto schemaField : tableSchemaFields) 
  {
    ss << "{" << schemaField->ToString() << "}," ;
  }
  LOG(INFO) << ss.str();
}

void TTable::PrintTable()
{
  std::stringstream ss;
  ss << "NumCols=" << NumColumns();
  ss << " NumRows=" << NumRows() << " Data=";

  // Print the table
  for (int64_t i=0; i<NumColumns(); i++)
  {
    auto chunkedArray = table_->column(i);
    //const std::shared_ptr<arrow::Field>& colField = schema_->field(i);
    //const std::shared_ptr<arrow::DataType>& colFieldType = colField->type();
       
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
          // todo
          //auto typeData = static_cast<decltype(colFieldType.get())>(data.get());
          //ss << typeData?typeData->ToString():"" << ",";
          if (data->is_valid)
            ss << data->ToString();
          ss << ",";
        }
      }
    }
  }
  LOG(INFO) << ss.str();
  google::FlushLogFiles(google::INFO);
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
  LOG(INFO) << ss.str();
}

// This gives a slice from offset from beginning of length length
std::shared_ptr<arrow::Table> TTable::Slice(int64_t offset, int64_t length) {
  auto arrTable = table_->Slice(offset, length);
  return arrTable;
}
