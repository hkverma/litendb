#include <common.h>
#include <TCoreTypes.h>

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

    /// Add a block to catalog
    /// @param block block to be added
    /// @id assigned uuid
    Status AddBlock(std::shared_ptr<TBlock> block,  Tguid::Uuid& id);

    
    /// Return true if block exists
    /// @param block to be checked in catalog
    /// @param id is the uuid
    bool IfExists(std::shared_ptr<TBlock> block,  Tguid::Uuid& id);

    /// Return true if block exists
    /// @param block to be checked in catalog
    bool IfExists(std::shared_ptr<TBlock> block);

    using TableNameColumnNamePair = std::pair<std::string, std::string>;
    using VersionToUuidMap = std::map<int, Tguid::Uuid>;
    
  private:

    /// A singleton class for catalog
    static std::shared_ptr<TCatalog> tCatalog_;
      
    /// Map an uuid to a block pointer
    std::unordered_map<Tguid::Uuid, std::shared_ptr<TBlock>> idToBlock_;
    
    /// Map a block pointer to a UUID
    std::unordered_map<std::shared_ptr<TBlock>, Tguid::Uuid> blockToId_;
    
    /// map table name and field_name to a map containing pairs of version and array UUIDs
    std::unordered_map<TableNameColumnNamePair, VersionToUuidMap> blockIds_;
    
    /// hash map table name to table information
    std::unordered_map<std::string, std::shared_ptr<TTable>> tables_;

    /// shared_lock
    mutable std::shared_mutex mutex_;

    /// TBD
    /// Get Id for given table name and field name
    /// @param blockName table and column name
    /// @param cacheId UUid for the given pair
    bool GetId(TableNameColumnNamePair blockName, Tguid::Uuid& cacheId);
    
  };
  
};
