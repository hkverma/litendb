#pragma once

#include <common.h>
#include <TCacheTypes.h>

// TBD persist schema and all data files in SSD using a separate object
namespace liten {
  
  
  class TCatalog {
  public:
    
    /// Get a singleton instance, if not present create one
    static std::shared_ptr<TCatalog> GetInstance();

    /// Add a block to catalog
    /// @param block block to be added
    /// @id assigned uuid
    TStatus AddBlock(std::shared_ptr<TBlock> block,  TGuid::Uuid& id);

    
    /// Return true if block exists
    /// @param block to be checked in catalog
    /// @param id is the uuid
    bool IfExists(std::shared_ptr<TBlock> block,  TGuid::Uuid& id);

    /// Return true if block exists
    /// @param block to be checked in catalog
    bool IfExists(std::shared_ptr<TBlock> block);

    /// Return information with compute information
    std::string GetTableInfo() const; //TBD bool schema=false, bool table=false) const;

    /// Get Id for given table name and field name
    /// @param tableName name of the table
    /// returns ptr to TTable, null if not present
    /// TBD Get Table by tableName 
    std::shared_ptr<TTable> GetTable(std::string tableName) const;
    // TBD Write an iterator here..
    std::unordered_map<std::string, std::shared_ptr<TTable>>& GetTableMap() { return tables_; }
    
    using TableNameColumnNamePair = std::pair<std::string, std::string>;
    using VersionToUuidMap = std::map<int, TGuid::Uuid>;

    /// Default destructor
    ~TCatalog() { }

  private:

    /// Cannot construct
    TCatalog() { }
    
    /// Allow shared_ptr with private constructors
    struct MakeSharedEnabler;    
    
    /// A singleton class for catalog
    static std::shared_ptr<TCatalog> tCatalog_;
      
    /// Map an uuid to a block pointer
    std::unordered_map<TGuid::Uuid, std::shared_ptr<TBlock>, hash_boost> idToBlock_;
    
    /// Map a block pointer to a UUID
    std::unordered_map<std::shared_ptr<TBlock>, TGuid::Uuid> blockToId_;
    
    /// map table name and field_name to a map containing pairs of version and array UUIDs
    std::unordered_map<TableNameColumnNamePair, VersionToUuidMap, hash_pair> blockIds_;
    
    /// hash map table name to table information
    std::unordered_map<std::string, std::shared_ptr<TTable>> tables_;

    /// shared_lock
    mutable std::shared_mutex mutex_;

    /// TBD
    /// Get Id for given table name and field name
    /// @param blockName table and column name
    /// @param cacheId UUid for the given pair
    bool GetId(TableNameColumnNamePair blockName, TGuid::Uuid& cacheId);
    
  };
  
  struct TCatalog::MakeSharedEnabler : public TCatalog {
    MakeSharedEnabler() : TCatalog() { }
  };
  
};
