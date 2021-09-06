#include <TColumn.h>
#include <TCatalog.h>
#include <TBlock.h>

namespace liten
{

// Add all blocks to catalog
/* TBD
TStatus TColumn::AddToCatalog() {
  for (int arrNum = 0; arrNum<chunkedArray_->num_chunks(); arrNum++)
  {
    std::shared_ptr<arrow::Array> arr = chunkedArray_->chunk(arrNum);
    auto block = TBlock::Create(arr);
    if (nullptr == block)
    {
      return TStatus::UnknownError("Cannor create a block.");
    }
  }
  return TStatus::OK();
}
*/
}
