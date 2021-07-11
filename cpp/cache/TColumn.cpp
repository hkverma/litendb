
/// Construct a column
    /// @param name of the column
    /// @param type if dimension or fact table
    /// @param chunkedArrow an arrow table that has been read 
TColumn::TColumn(std::string name,
                 Type type,
                 std::shared_ptr<arrow::ChunkedArray> chunkedArray)
  : type_(type), name_(name), chunkedArray_(chunkedArray)
{
}

// Add all blocks to catalog
Status TColumn::AddToCatalog() {
  for (int arrNum = 0; arrNum<chunkedArray_->num_chunks(); arrNum++)
  {
    auto block = make_shared<TBlock>(chArr->chunk(arrNum));
    Tguid::Uuid id;
    Status status = std::move(tCatalog_->AddBlock(block, id));
    if (!status.ok()) {
      return status;
    }
  }
  return Status::OK();
}
