#pragma once

#include <memory>

namespace liten
{

/// All cache related configurations TBD make it a generic key-value property list
class TConfigs
{
public:

  /// Get a singleton instance, if not present create one
  static std::shared_ptr<TConfigs> GetInstance();

  /// Return information with compute information
  std::string GetComputeInfo();
    
  ~TConfigs() { }
    
private:

  /// Should be constructed once at startup
  TConfigs() { }
    
  /// singleton member
  static std::shared_ptr<TConfigs> tConfigs_;
    
  /// Allow shared_ptr with private constructors
  struct MakeSharedEnabler;    
    
};
  
struct TConfigs::MakeSharedEnabler : public TConfigs {
  MakeSharedEnabler() : TConfigs() { }
};
  

}
