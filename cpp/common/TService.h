#pragma once

#include <TLog.h>

namespace liten
{

/// Starts all liten services, performs startup and shutdown operations
class TService {
public:
    
  // On one node, there can only be one service
  static std::shared_ptr<TService> GetInstance();
    
  /// Start all services and utilities
  void Start();

  /// Shutdown all services and utilities
  void Shutdown();

private:

  /// Only one service per node
  TService() { }

  /// Liten Service singleton element
  static std::shared_ptr<TService> tService_;

  /// Logging utility
  std::shared_ptr<TLog> tLog_;

  /// Allow shared_ptr with private constructors
  struct MakeSharedEnabler;
    
};

struct TService::MakeSharedEnabler : public TService {
  MakeSharedEnabler() : TService() { }
};  

};
