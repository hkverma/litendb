#include <TColumn.h>
#include <TCatalog.h>
#include <TBlock.h>

using namespace liten;

// Add all blocks to catalog
Status TColumn::AddToCatalog() {
  for (int arrNum = 0; arrNum<chunkedArray_->num_chunks(); arrNum++)
  {
    std::shared_ptr<arrow::Array> arr = chunkedArray_->chunk(arrNum);
    auto block = std::make_shared<TBlock>(arr);
    TGuid::Uuid id;
    Status status = std::move(TCatalog::GetInstance()->AddBlock(block, id));
    if (!status.ok()) {
      return status;
    }
  }
  return Status::OK();
}
