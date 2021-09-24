#include <TSchema.h>
#include <TCatalog.h>

namespace liten
{

std::vector<std::string> FieldTypeString = {"DimensionField", "MetricField", "FeatureField", "EmbeddingField"};

TResult<std::shared_ptr<TSchema>> TSchema::Create(std::string name,
                                                  TableType type,
                                                  std::shared_ptr<arrow::Schema> schema)
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
      TLOG(INFO) << "Created using an already existing schema by name=" << name;
      return tschema;
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
    return status;
  }
  // By default all fields are metric, Join fields to create dim fields
  const arrow::FieldVector& fv = schema->fields();
  for (auto field: fv)
  {
    tschema->typeFields_[field] = (DimensionTable == tschema->type_)?FeatureField:MetricField;
  }
  return tschema;
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
  parentSchema->AddChildField(parentField, shared_from_this(), field);
  
  typeFields_[field] = DimensionField;
  
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
  parentFields_[field] = std::make_pair(parentSchema, parentField);
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
  childFields_[field] = std::make_pair(childSchema, childField);
}

TStatus TSchema::SetFieldType(std::string fieldName, FieldType fieldType)
{
  std::shared_ptr<arrow::Field> field = schema_->GetFieldByName(fieldName);
  if (nullptr == field)
  {
    return TStatus::Invalid("No field found by name=", fieldName);
  }
  typeFields_[field] = fieldType;
  return TStatus::OK();
}

TResult<FieldType> TSchema::GetFieldType(std::shared_ptr<arrow::Field> field) const
{
  auto fieldItr = typeFields_.find(field);
  if (typeFields_.end() == fieldItr)
  {
    return TStatus::UnknownError("No field in liten schema found by name=", field->name());
  }
  return TResult<FieldType>(fieldItr->second);
}

TResult<FieldType> TSchema::GetFieldType(std::string fieldName) const
{
  std::shared_ptr<arrow::Field> field = std::move(schema_->GetFieldByName(fieldName));
  if (nullptr == field)
  {
    return TStatus::Invalid("No field found by name=", fieldName);
  }
  auto result = std::move(GetFieldType(field));
  return result;
}

TResult<FieldType> TSchema::GetFieldType(int32_t colNum) const
{
  if (colNum < 0 || colNum>schema_->num_fields())
  {
    return TStatus::Invalid("Incorrect column number=", colNum);
  }
  auto result = std::move(GetFieldType(schema_->field(colNum)));
  return result;
}

// Schema Json representation
std::string TSchema::ToString()
{
  try {
    auto& fv = schema_->fields();
    json fields;
    for (auto f: fv)
    {
      json fvals;
      fvals["datatype"] = f->type()->ToString();
      fvals["type"] = FieldTypeString[typeFields_[f]];
      fields[f->name()] = fvals;
    }
    json sch;
    sch["name"] =  name_;
    sch["type"] = TableTypeString[type_];
    sch["fields"] = fields;
    if (parentFields_.size() > 0)
    {
      json parents;
      for (auto& pf : parentFields_)
      {
        parents[pf.first->name()] = {pf.second.first->GetName(),pf.second.second->name()};
      }
      sch["parent"] = parents;
    }
    if (childFields_.size() > 0)
    {
      json child;
      for (auto& pf : childFields_)
      {
        child[pf.first->name()] = {pf.second.first->GetName(),pf.second.second->name()};
      }
      sch["child"] = child;
    }
    json tsch;
    tsch["schema"] = sch;
    std::stringstream ss;
    ss << std::setw(1) << tsch;
    return std::move(ss.str());
  }
  catch (std::exception& exc)
  {
    TLOG(ERROR) << "Failed to create json for schema " << name_ << "Error=" << exc.what();
    return "";
  }
}

TResult<TSchema::SchemaField> TSchema::GetParentField(int i)
{
  if (i < 0 || i > schema_->num_fields())
  {
    return TStatus::IndexError("Schema out of index=", i);
  }
  auto itr = parentFields_.find(schema_->field(i));
  if (parentFields_.end() != itr)
  {
    return itr->second;
  }
  return std::make_pair(nullptr,nullptr);
}

TResult<TSchema::SchemaField> TSchema::GetParentField(const std::string& fieldName) const
{
  std::shared_ptr<arrow::Field> field = schema_->GetFieldByName(fieldName);
  if (nullptr == field)
  {
    return TStatus::IndexError("Schema incorrect field name=", fieldName);
  }
  auto itr = parentFields_.find(field);
  if (parentFields_.end() != itr)
  {
    return itr->second;
  }
  return std::make_pair(nullptr,nullptr);
}

TResult<TSchema::SchemaField> TSchema::GetChildField(int i) const
{
  if (i < 0 || i > schema_->num_fields())
  {
    return TStatus::IndexError("Schema out of index=", i);
  }
  auto itr = childFields_.find(schema_->field(i));
  if (parentFields_.end() != itr)
  {
    return itr->second;
  }
  return std::make_pair(nullptr,nullptr);
}

TResult<TSchema::SchemaField> TSchema::GetChildField(const std::string& fieldName) const
{
  std::shared_ptr<arrow::Field> field = schema_->GetFieldByName(fieldName);
  if (nullptr == field)
  {
    return TStatus::IndexError("Schema incorrect field name=", fieldName);
  }
  auto itr = childFields_.find(field);
  if (childFields_.end() != itr)
  {
    return itr->second;
  }
  return std::make_pair(nullptr,nullptr);
}

TStatus TSchema::AddTable(std::shared_ptr<TTable> ttable)
{
  tables_.insert(ttable);
  return TStatus::OK();
}

}
