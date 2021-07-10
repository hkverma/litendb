
std::shared_ptr<TCatalog> TCatalog::tCatalog_=nullptr;

// Add Block to Cache  
Status TCatalog::AddBlock(std::shared_ptr<TBlock> block, boost::uuids::uuid& id)
{
  if (nullptr == block)
  {
    return Status::KeyError("Catalog cannot add null Block");    
    return Status::OK();
  }
  auto 
  if (tCatalog_->IfExists(block))
  {
    return Status::KeyError("Block already exists in catalog");
  }
  
  Status status = std::move(tCatalog_->AddBlock(block, id));
  return status;
}
