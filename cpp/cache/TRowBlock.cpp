#include <TRowBlock.h>
#include <TBlock.h>
#include <TSchema.h>

namespace liten
{

// Construct a RowBlock from a vector of blocks
TResult<std::shared_ptr<TRowBlock>> TRowBlock::Create(std::shared_ptr<TTable> ttable,
                                                      std::vector<std::shared_ptr<TBlock>>& blocks,
                                                      int64_t numRows)
{
  if (blocks.size() <= 0)
  {
    return TResult<std::shared_ptr<TRowBlock>>(TStatus::KeyError("Zero blocks in RowBlock create"));
  }
  numRows = (numRows<0)?blocks[0]->GetArray()->length():numRows;
  assert(ttable);
  
  auto trb = std::make_shared<MakeSharedEnabler>();
  trb->ttable_ = ttable;
  trb->numRows_ = numRows;
    
  for (auto blk : blocks)
  {
    trb->blocks_.push_back(blk);
    assert(numRows <= blk->GetArray()->length());
  }

  return trb;
}

// Construct a rowblock from arrow rowblocks
TResult<std::shared_ptr<TRowBlock>> TRowBlock::Create(std::shared_ptr<TTable> ttable,
                                                      std::shared_ptr<arrow::RecordBatch> recordBatch,
                                                      int64_t numRows)

{
  std::vector<std::shared_ptr<arrow::Array>> arrs = std::move(recordBatch->columns());
  if (arrs.size() <= 0)
  {
    return TResult<std::shared_ptr<TRowBlock>>(TStatus::KeyError("Zero blocks in RowBlock create"));
  }
  numRows = (numRows<0)?arrs[0]->length():numRows;
  assert(ttable);
  
  auto trb = std::make_shared<MakeSharedEnabler>();
  trb->ttable_ = ttable;
  trb->numRows_ = numRows;
  
  for (auto arr : arrs)
  {
    auto blk = TBlock::Create(arr);
    trb->blocks_.push_back(blk);
    assert(numRows <= blk->GetArray()->length());
  }
  return trb;
}

}
