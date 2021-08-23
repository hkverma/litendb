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

  /// Get allowed worker threads
  int64_t GetWorkerThreads()
  {
    return numWorkerThreads_;
  }

  /// Return information with compute information
  std::string GetComputeInfo()
  {
    std::string str("{\"WorkerThreads\":");
    str.append(std::to_string(numWorkerThreads_)).append("}");
    return std::move(str);
  }
        
    
  ~TConfigs() { }
    
private:

  /// Should be constructed once at startup
  TConfigs() { }
    
  /// singleton member
  static std::shared_ptr<TConfigs> tConfigs_;
    
  /// Total worker CPU nodes
  int64_t numWorkerThreads_ = 6;

  /// Allow shared_ptr with private constructors
  struct MakeSharedEnabler;    
    
};
  
struct TConfigs::MakeSharedEnabler : public TConfigs {
  MakeSharedEnabler() : TConfigs() { }
};
  

}
