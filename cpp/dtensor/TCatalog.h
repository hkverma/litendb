namespace liten {

  class TCatalog {
  public:
    /// Get a singleton instance, if not present create one
    static std::shared_ptr<TCatalog> GetInstance()
    {
      if (nullptr == tCatalog_)
      {
        tCatalog_ = std::make_shared<TCatalog>();
      }
      TLOG(INFO) << "Created a new TCatalog";
      return tCatalog_;
    }
    
  private:

    /// A singleton class for catalog
    static std::shared_ptr<TCatalog> tCatalog_;
      
    /// Map an array uuid to an array pointer
    std::unordered_map<boost::uuids::uuid, std::shared_ptr<TBlock>> blocks_;
    
    /// map table name and field_name to a map containing pairs of version and array UUIDs
    std::unordered_map<TableNameColumnNamePair, VersionToUuidMap> blockIds_;
    
    /// hash map table name to table information
    std::unordered_map<std::string, std::shared<TTable>> tables_;
        
  };
  
};
