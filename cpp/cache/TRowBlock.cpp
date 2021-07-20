#include <TRowBlock.h>
#include <TBlock.h>
#include <TSchema.h>

namespace liten
{

// Construct a rowblock from a vector of blocks
std::shared_ptr<TRowBlock> TRowBlock::Create(TableType type,
                                             std::shared_ptr<TSchema> schema,
                                             int64_t num_rows,
                                             std::vector<std::shared_ptr<TBlock>>& columns)
{
  std::vector<std::shared_ptr<arrow::Array>> arrColumns;
  for (auto blk : columns)
  {
    auto arr = blk->GetArray();
    arrColumns.push_back(arr);
    assert(num_rows <= arr->length());
  }
  auto rb = arrow::RecordBatch::Make(schema->GetSchema(), num_rows, arrColumns);
  if (nullptr == rb)
  {
    return nullptr;
  }
  auto trb = std::make_shared<MakeSharedEnabler>();
  trb->type_ = type;
  trb->recordBatch_ = rb;
  return trb;
}

}
