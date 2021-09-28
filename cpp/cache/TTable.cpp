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
  auto ttable = TCatalog::GetInstance()->GetTable(tableName);
  if (ttable)
  {
    return TStatus::AlreadyExists("Table ", tableName, " already exists.");
  }
  if (schemaName.empty()) {
    schemaName = tableName+"_schema";
  }
  ttable = std::make_shared<MakeSharedEnabler>();
  ttable->name_ = std::move(tableName);
  ttable->schema_ = nullptr;
  ttable->type_ = type;
  ttable->schemaName_ = schemaName;
  // Add this table to catalog
  TStatus status = TCatalog::GetInstance()->AddTable(ttable);
  if (!status.ok())
  {
    return status;
  }
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
TStatus TTable::CreateMaps()
{
  TStatus status;
  for (int64_t cnum=0; cnum<NumColumns(); cnum++)
  {
    auto schemaFieldResult = std::move(schema_->GetParentField(cnum));
    if (!schemaFieldResult.ok())
    {
      return TStatus::Invalid("Invalid parent schema field");
    }
    auto schemaField = schemaFieldResult.ValueOrDie();
    auto tschema = schemaField.first;
    auto field = schemaField.second;
    if (nullptr != tschema && nullptr != field)
    {
      int fieldId = tschema->GetSchema()->GetFieldIndex(field->name());
      if (fieldId != -1)
      {
        // TBD
        continue;
      }
    }

    std::shared_ptr<TColumn> col = GetColumn(cnum);
    auto colResult = TColumnMap::Create(col);
    if (!colResult.ok())
    {
      return colResult.status();
    }
    auto colMap = colResult.ValueOrDie();

    status = colMap->CreateZoneMap();
    // TBD reverseMap only for dimension tables
    status = colMap->CreateReverseMap();
  }
  return status;
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
TResult<std::shared_ptr<TRowBlock>> TTable::AddRowBlock(std::shared_ptr<arrow::RecordBatch> arrb,
                                                           int64_t numRows)
{
  // Create rowblock
  auto rb_result = std::move(TRowBlock::Create(shared_from_this(), arrb, numRows));
  if (!rb_result.ok())
  {
    return std::move(rb_result);
  }
  auto rb = rb_result.ValueOrDie();

  // Check and create schema
  if (nullptr == schema_)
  {
    auto tschResult = AddSchema(arrb->schema());
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

  // columns should now be the same
  if (rb->NumColumns() != columns_.size())
  {
    return TStatus::Invalid("Different number of columns in rowblock=", rb->NumColumns(),
                            "and table columns=", columns_.size(), " in table ", name_);
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
TResult<std::shared_ptr<TSchema>> TTable::AddSchema(std::shared_ptr<arrow::Schema> schema)
{

  if (schemaName_.empty()) {
    schemaName_ = name_+"_schema";
  }
  std::shared_ptr<TSchema> tschema = TCatalog::GetInstance()->GetSchema(schemaName_);
  if (nullptr == tschema)
  {
    auto tschemaResult = TSchema::Create(schemaName_, type_, schema);
    if (!tschemaResult.ok())
    {
      return TStatus::AlreadyExists("In Table=",name_," schema=", schema_->GetName(), " failed to create with msg=", tschemaResult.status().message());
    }
    tschema = tschemaResult.ValueOrDie();
  }

  // TBD what if schema is not null
  schema_ = tschema;
  columns_.resize(schema->num_fields());
  parentColumns_.resize(schema->num_fields());
  parentColumnId_.resize(schema->num_fields());
  for (auto i=0; i<schema->num_fields(); i++) {
    columns_[i] = std::make_shared<TColumn>(shared_from_this(), schema->field(i));
    parentColumns_[i] = nullptr;
    parentColumnId_[i] = 0;
  }

  // Add this table to schema
  schema_->AddTable(shared_from_this());

  return schema_;

}

std::shared_ptr<TBlock> TTable::GetBlock(int64_t rbNum, int64_t colNum)
{
  auto rblk = GetRowBlock(rbNum);
  if (!rblk) return nullptr;
  auto blk = rblk->GetBlock(colNum);
  return blk;
}

// For this table - create all tensor structures
//  If a column is of dimension type
//    If child field exists - create reverseMap for all the dimension fields
//    If parent field exists - create joinMap into the parent field table
TStatus TTable::CreateTensor()
{
  TStatus status;
  std::string errmsg;

  // Return field if field type is fieldType, adds any error message to errmsg
  auto ifFieldType = [&](int32_t cnum, FieldType fieldType) -> bool
  {
    if (nullptr == schema_)
    {
      errmsg.append("No schema with table=").append(name_);
      return false;
    }
    auto fieldResultStatus = schema_->GetFieldType(cnum);
    if (!fieldResultStatus.ok())
    {
      errmsg.append(StringBuilder("Invalid field type for col=", cnum, " msg=",
                                  fieldResultStatus.status().message(), "; "));
      return false;
    }
    auto curFieldType = fieldResultStatus.ValueOrDie();
    if (curFieldType != fieldType)
      return false;
    return true;
  };

  auto ifValidFieldResult = [&](TResult<TSchema::SchemaField>& schemaFieldResult,
                                std::shared_ptr<arrow::Field> field) -> bool
  {
    if (!schemaFieldResult.ok())
    {
      errmsg.append(StringBuilder("Invalid child schema field ",
                                  schemaFieldResult.status().message(), "; "));
      return false;
    }
    auto schemaField = schemaFieldResult.ValueOrDie();
    auto tschema = schemaField.first;
    field = schemaField.second;
    if (nullptr == tschema || nullptr == field)
    {
      errmsg.append("schema or field are null in schemaField pair; ");
      return false;
    }
    auto resultField = tschema->GetFieldType(field);
    if (!resultField.ok())
    {
      errmsg.append("Incorrect field in schemaField pair; ");
      return false;
    }
    auto fieldType = resultField.ValueOrDie();
    // Only for Dimension fields create tensors
    if (DimensionField != fieldType)
      return false;
    return true;
  };

  for (int64_t cnum=0; cnum<NumColumns(); cnum++)
  {


    // Get current columns
    std::shared_ptr<TColumn> col = GetColumn(cnum);
    if (nullptr == col)
    {
      errmsg.append("Null col in tensor creation; ");
      continue;
    }

    // If not dimension field continue
    if (!ifFieldType(cnum, DimensionField))
      continue;

   // If child field exists create a reverse lookup column
    std::shared_ptr<arrow::Field> field;
    auto schemaFieldResult = std::move(schema_->GetChildField(cnum));
    if (ifValidFieldResult(schemaFieldResult, field))
    {
      if (nullptr == col->GetMap())
      {
        auto colResult = TColumnMap::Create(col);
        if (colResult.ok())
        {
          auto colMap = colResult.ValueOrDie();
          status = colMap->CreateReverseMap();
          if (colMap->IfValidReverseMap())
          {
            errmsg.append("Could not create a valid reverse map; ");
          }
        }
        else
        {
          errmsg.append("Could not create column map; ");
        }
      }
    }

    // If parent field exists, create a forward lookup column
    schemaFieldResult = std::move(schema_->GetParentField(cnum));
    if (ifValidFieldResult(schemaFieldResult, field))
    {
      status = CreateColumnLookUp(cnum, col, field);
      if (!status.ok())
      {
        errmsg.append("column creation failed with msg=").append(status.message()).append("; ");
      }
    }
  }

  // Non empty errmsg means that some tensors were not properly created
  if (errmsg.empty())
    return TStatus::OK();
  return TStatus::Invalid(errmsg);
}

// TBD $$$$
TStatus TTable::CreateColumnLookUp(int64_t cnum,
                                   std::shared_ptr<TColumn> col,
                                   std::shared_ptr<arrow::Field> field)
{

  auto joinCol = std::make_shared<TColumn>(shared_from_this(), field);
  for (auto i=0; i<col->NumBlocks(); i++)
  {
    auto arr = col->GetBlock(i)->GetArray();

    // Build an array to join the data directly
    //std::unique_ptr<arrow::ArrayBuilder> arrBuilder = std::make_unique(arrow::ArrayBuilder);
    //auto arrBuilder = arrow::MakeBuilder(arrow::DefaultMemoryPool, arr->type(), &arrBuilder);

    // $$$$$
    // Iterate through the arrays of current column
    // Look up reverse and create another array using MakeBuilder
    // Add it to the column list now
    // This is the pre-join step
    //
  }
  return TStatus::OK();
}

}
