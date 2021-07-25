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
                      std::shared_ptr<TSchema> parentSchema,
                      std::string parentFieldName)
{
  if (nullptr == parentSchema)
  {
    return TStatus::Invalid("Join Schema is null");
  }
  std::shared_ptr<arrow::Field> field = schema_->GetFieldByName(fieldName);
  if (nullptr == field)
  {
    return TStatus::Invalid("Field name ", fieldName, " for schema ", name_, " does not exist.");
  }
  std::shared_ptr<arrow::Field> parentField = parentSchema->GetSchema()->GetFieldByName(parentFieldName);
  if (nullptr == parentField)
  {
    return TStatus::Invalid("Field name ", parentFieldName, " for schema ", parentSchema->GetName(), " does not exist.");
  }

  AddParentField(field, parentSchema, parentField);
  AddChildField(parentField, shared_from_this(), field);

  return TStatus::OK();
}

void TSchema::AddParentField(std::shared_ptr<arrow::Field> field,
                             std::shared_ptr<TSchema> parentSchema,
                             std::shared_ptr<arrow::Field> parentField)
{
  auto itr = parentFields_.find(parentField);
  if (parentFields_.end() != itr)
  {
    TLOG(INFO) << "Field name " << field->name() << " for schema " << parentSchema->GetName() << " already exists. Will be overwritten";
  }
  parentFields_[parentField] = std::make_pair(parentSchema, parentField);
}

void TSchema::AddChildField(std::shared_ptr<arrow::Field> field,
                            std::shared_ptr<TSchema> childSchema,
                            std::shared_ptr<arrow::Field> childField)
{
  auto itr = childFields_.find(childField);
  if (childFields_.end() != itr)
  {
    TLOG(INFO) << "Field name " << field->name() << " for schema " << name_ << " already exists. Will be overwritten";
  }
  childFields_[childField] = std::make_pair(childSchema, childField);
}

// Schema is an object TBD
std::string TSchema::ToString()
{
  std::stringstream ss;
  ss << "{\n";
  ss << " schema\":\n";
  ss << " {\"name\":\"" << name_; //<< "\",\n" ;
  ss << "  \"type\":\"" << TableTypeString[type_]<< "\",\n";  
  ss << "  \"fields\":\n";
  ss << "   {name:,\n";
  ss << "    datatype:,}}\n";
  return ss.str();
}
}
