
/// Construct a column
    /// @param name of the column
    /// @param type if dimension or fact table
    /// @param chunkedArrow an arrow table that has been read 
TColumn::TColumn(std::string name,
                 Type type,
                 std::shared_ptr<arrow::ChunkedArray> chunkedArray)
  : type_(type), name_(name), chunkedArray_(chunkedArray)
{
    for (int arrNum = 0; chunkedArray_->num_chunks(); arrNum++)
    {
      auto tBlock = make_shared<TTable>(chArr->chunk(arrNum));
      
      ss << " Arr " << arrNum << " Size=" << arr->length();
      ss << " Type=" << arr->type()->ToString() ;
      ss << " Min=";
      colMap->GetMin(arrNum,minVal)?(ss << minVal):(ss << "None");
      ss << " Max=";
      colMap->GetMax(arrNum,maxVal)?(ss << maxVal):(ss << "None");
      ss << ";" ;
    }
  
}
