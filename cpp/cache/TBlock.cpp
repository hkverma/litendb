#include <TBlock.h>
#include <TCatalog.h>

namespace liten
{

std::unordered_map<std::shared_ptr<arrow::Array>, std::shared_ptr<TBlock>> TBlock::arrayToBlock_;
std::shared_mutex TBlock::arrayToBlockMutex_;


std::shared_ptr<TBlock> TBlock::Create(std::shared_ptr<arrow::Array> arr)
{
  auto tblk = GetTBlock(arr);
  if (tblk)
  {
    return tblk;
  }
  tblk = std::make_shared<TBlock::MakeSharedEnabler>();
  tblk->arr_ = arr;
  TStatus status = std::move(TCatalog::GetInstance()->AddBlock(tblk, tblk->id_));
  if (!status.ok())
  {
    TLOG(ERROR) << "Cannot create uuid for block. " << status.message();
    tblk = nullptr;
  } else {
    status = std::move(AddTBlock(tblk));
    if (!status.ok())
    {
      TLOG(ERROR) << "Cannot add arrow to block key.";
      tblk = nullptr;
    }
  }
  return tblk;
}

// Get TBlock for a given array
std::shared_ptr<TBlock> TBlock::GetTBlock(std::shared_ptr<arrow::Array> arr)
{
  std::shared_lock<std::shared_mutex> lk(arrayToBlockMutex_);
  auto it = arrayToBlock_.find(arr);
  if (arrayToBlock_.end() == it)
    return nullptr;
  return it->second;
}

// Add TBlock to the lookup list
TStatus TBlock::AddTBlock(std::shared_ptr<TBlock> blk)
{
  std::unique_lock<std::shared_mutex> lk(arrayToBlockMutex_);
  auto it = arrayToBlock_.find(blk->arr_);
  if (arrayToBlock_.end() == it)
  {
    arrayToBlock_[blk->arr_] = blk;
  }
  return TStatus::OK();
}

}
