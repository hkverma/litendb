#include <TBlock.h>
#include <TCatalog.h>

namespace liten
{

std::unordered_map<std::shared_ptr<arrow::Array>, std::shared_ptr<TBlock>> TBlock::arrayToBlock_;
std::shared_mutex TBlock::arrayToBlockMutex_;

TResult<std::shared_ptr<TBlock>> TBlock::Create(std::shared_ptr<arrow::Array> arr)
{
  auto tblk = GetTBlock(arr);
  if (tblk)
  {
    return tblk;
  }
  tblk = std::make_shared<TBlock::MakeSharedEnabler>();
  tblk->arr_ = arr;
  TStatus status = std::move(TCatalog::GetInstance()->AddBlock(tblk, tblk->id_));
  LITEN_RETURN_IF(!status.ok(), status);
  status = std::move(AddTBlock(tblk));
  LITEN_RETURN_IF(!status.ok(), status);  
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

std::string TBlock::ToString()
{
  std::stringstream ss;
  auto arr = GetArray();
  ss << " Arr Size=" << arr->length();
  ss << " Type=" << std::move(arr->type()->ToString()) << " ";
  for (auto rn=0; rn<arr->length(); rn++)
  {
    arrow::Result<std::shared_ptr<arrow::Scalar>> dataResult = arr->GetScalar(rn);
    if ( !dataResult.ok() )
    {
      ss << ",";
    }
    else
    {
      std::shared_ptr<arrow::Scalar> data = dataResult.ValueOrDie();
      if (data->is_valid)
        ss << data->ToString();
      ss << ",";
    }
  }
  return std::move(ss.str());
}  

}

