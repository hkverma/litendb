#pragma once
#include <common.h>
#include <cache_fwd.h>
//
// Liten Columnar Storage Node
//
// RowBlock with all blocks for a row of table
// TBD Add versions here for transational support
//

namespace liten
{

class TRowBlock {
public:
  // Create a rowblock from a vector of blocks
  // @param type specify if part of fact or dimension
  // @param schema schema of the row
  // @num_row number of rows, must be same rows across all Blocks
  // @columns vector of all blocks to be added
  static std::shared_ptr<TRowBlock> Create(TableType type,
                                           std::shared_ptr<TSchema> schema,
                                           int64_t num_rows,
                                           std::vector<std::shared_ptr<TBlock>>& columns);
  ~TRowBlock() { }

private:
    
  /// Use only named constructor
  TRowBlock() { }
    
  /// Type of rowblock -fact or dimension
  TableType type_;

  /// All rowblocks are recordbatches
  std::shared_ptr<arrow::RecordBatch> recordBatch_;

  /// Allow shared_ptr with private constructors
  struct MakeSharedEnabler;
    
};
  
struct TRowBlock::MakeSharedEnabler : public TRowBlock
{
  MakeSharedEnabler() : TRowBlock() { }
};
  
}
