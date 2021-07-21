#include <TSchema.h>
#include <TCatalog.h>

namespace liten
{

TResult<std::shared_ptr<TSchema>> TSchema::Create(std::shared_ptr<arrow::Schema> schema,
                                                  TableType type,
                                                  std::string name)
{
  auto catalog = TCatalog::GetInstance();

  // Check if already exists in catalog
  auto tschema = catalog->GetSchema(name);
  if (nullptr != tschema)
  {
    if (tschema->schema_ == schema &&
        tschema->type_ == type &&
        tschema->name_ == name)
    {
      TLOG(INFO) << "Creating an already existing schema by name=" << name;
      return TResult<std::shared_ptr<TSchema>>(tschema);
    }
    return TStatus::AlreadyExists("Schema name=", name, " is already in use.");
  }
  // Create and add to catalog
  tschema = std::make_shared<MakeSharedEnabler>();
  tschema->schema_ = schema;
  tschema->type_ = type;
  tschema->name_ = std::move(name);
  TStatus status = catalog->AddSchema(tschema);
  if (!status.ok())
  {
    return TResult<std::shared_ptr<TSchema>>(status);
  }
  return TResult<std::shared_ptr<TSchema>>(tschema);
}

std::shared_ptr<arrow::Schema> TSchema::GetSchema()
{
  return schema_;
}

TStatus TSchema::Join(std::string fieldName,
                      std::shared_ptr<TSchema> childSchema,
                      std::string childFieldName)
{
  if (nullptr == childSchema)
  {
    return TStatus::Invalid("Child Schema is null");
  }
  std::shared_ptr<arrow::Field> parentField = schema_->GetFieldByName(fieldName);
  if (nullptr == parentField)
  {
    return TStatus::Invalid("Field name ", fieldName, " for schema ", name_, " does not exist.");
  }
  std::shared_ptr<arrow::Field> childField = schema_->GetFieldByName(childFieldName);
  if (nullptr == childField)
  {
    return TStatus::Invalid("Field name ", childFieldName, " for schema ", childSchema->GetName(), " does not exist.");
  }
  auto itr = joinColumns_.find(parentField);

  if (joinColumns_.end() != itr)
  {
    TLOG(INFO) << "Field name " << fieldName << " for schema " << " already exists. Will be overwritten";
  }
  joinColumns_[parentField] = std::make_pair(childSchema, childField);
  return TStatus::OK();
}

}
