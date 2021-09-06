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
  static std::shared_ptr<TRowBlock> Create(std::shared_ptr<TTable> ttable,
                                           std::vector<std::shared_ptr<TBlock>>& blocks,
                                           int64_t numRows=-1);                                           
  
  static std::shared_ptr<TRowBlock> Create(std::shared_ptr<TTable> ttable,
                                           std::shared_ptr<arrow::RecordBatch> rb,
                                           int64_t numRows=-1);
  
  ~TRowBlock() { }

  int64_t NumColumns();

  int64_t NumRows();

private:
    
  /// Use only named constructor
  TRowBlock() { }
    
  /// All RowBlocks must belong to a table
  std::shared_ptr<TTable> ttable_;

  /// All rowblocks are a vector of TBlocks
  std::vector<std::shared_ptr<TBlock>> blocks_;

  /// Pick first numRows from the rowblock
  int64_t numRows_;

  /// Allow shared_ptr with private constructors
  struct MakeSharedEnabler;
    
};
  
struct TRowBlock::MakeSharedEnabler : public TRowBlock
{
  MakeSharedEnabler() : TRowBlock() { }
};
  
}
