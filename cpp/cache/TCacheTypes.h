#pragma once

namespace liten
{
/// Tables are either dimension or fact tables
enum TableType {DimensionTable=0, FactTable};
extern std::vector<std::string> TableTypeString;

/// Schema field types are either dimension fields or Metric
enum FieldType {DimensionField=0, MetricField};
extern std::vector<std::string> FieldTypeString;

/// These are all defined classes in core library
class TCatalog;
class TColumn;
class TRowBlock;
class TBlock;
class TSchema;
class TTable;
class TColumnMap;

}
