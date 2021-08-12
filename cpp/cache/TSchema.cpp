#include <TSchema.h>
#include <TCatalog.h>

namespace liten
{

std::vector<std::string> FieldTypeString = {"dim", "metric"};

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
  // By default all fields are metric, Join fields to create dim fields
  const arrow::FieldVector& fv = schema->fields();
  for (auto field: fv)
  {
    tschema->typeFields_[field] = MetricField;
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
  
  typeFields_[field] = DimensionField;
  parentSchema->typeFields_[parentField] = DimensionField;
  
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

}
