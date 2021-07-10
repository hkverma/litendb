//
// Liten Columnar Storage Node
//
// RowBlock with all blocks for a row of table
// TBD Add versions here for transational support
//
#pragma once

#include <vector>
#include <iostream>
#include <set>

#include <TTable.h>
#include <common.h>

namespace liten {

  class TRowBlock {
  public:
    
  private:    
    /// Type of rowblock -fact or dimension
    Table::Type type_;
    
  };
};
