#pragma once

#include <common.h>
#include <cache_fwd.h>

namespace liten
{

/// Wrap arrow schema with fact, dimension and join columns
class TSchema : public std::enable_shared_from_this<TSchema>
{
public:

  /// Create TSChema from Arrow array and other informations
  /// @param name name of schema, is cached by this name
  /// @param type type of schema is for fact table or dim table
  /// @param schema Arrow schema
  static TResult<std::shared_ptr<TSchema>> Create(std::string name,
                                                  TableType type,
                                                  std::shared_ptr<arrow::Schema> schema);

  /// Destruct the array, nothing here for now
  ~TSchema() { }

  /// Get the name of the schema
  std::string GetName() const { return name_; }

  /// Get the table type
  TableType GetType() const { return type_; }

  /// Get Raw Array, Use it judiciously, prefer to add an access method
  std::shared_ptr<arrow::Schema> GetSchema();

  using SchemaField = std::pair<std::shared_ptr<TSchema>, std::shared_ptr<arrow::Field>>;

  /// joins this schema (child) field_id to parent[schema, field_id]
  TStatus Join(std::string fieldName,
               std::shared_ptr<TSchema> parentSchema,
               std::string parentFieldName);

  /// JSon string represntation for schema
  std::string ToString();

private:

  /// Arrow array
  std::shared_ptr<arrow::Schema> schema_;

  /// Type of table defined by this schema
  TableType type_;

  /// Field types either dim or metric
  std::map<std::shared_ptr<arrow::Field>, FieldType> typeFields_;

  /// Join columns - joins this schema (child) field_id to parent[schema, field_id]
  std::map<std::shared_ptr<arrow::Field>, SchemaField> parentFields_;

  /// Add parent fields to the schema
  void AddParentField(std::shared_ptr<arrow::Field> field,
                      std::shared_ptr<TSchema> parentSchema,
                      std::shared_ptr<arrow::Field> parentField);

  /// Join columns - list all child schema fields here
  std::map<std::shared_ptr<arrow::Field>, SchemaField> childFields_;

  /// Add child fields to the schema
  void AddChildField(std::shared_ptr<arrow::Field> field,
                     std::shared_ptr<TSchema> childSchema,
                     std::shared_ptr<arrow::Field> childField);

  /// Provide a unique name for the schema
  std::string name_;

  /// Use only named constructor
  TSchema() { }

  /// Allow shared_ptr in static create return
  struct MakeSharedEnabler;
};

struct TSchema::MakeSharedEnabler : public TSchema
{
  MakeSharedEnabler() : TSchema() { }
};

}
