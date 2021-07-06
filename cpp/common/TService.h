#pragma once

#include <TLog.h>

namespace liten
{

  /// Starts all liten services, performs startup and shutdown operations
  class TService {
  public:
    
    // On one worker node, there can only be one service
    std::shared_ptr<TService> GetInstance()
    {
      if (nullptr == tService_)
      {
        tService_ = std::make_shared<TService>();
      }
      return tService_;
    }
    
    // Start all services and utilities
    void Start()
    {
      tLog_ = TLog::GetInstance()->Start();
      TLOG(INFO) << "Start Liten Services";
    };

    /// Shutdown all services and utilities
    void Shutdown()
    {
      TLOG(INFO) << "Stop Liten Services";
      tLog_->Stop();
    }
    
  private:

    /// Liten Service singleton element
    static std::shared_ptr<TService> tService_;

    /// Logging utility
    std::shared_ptr<TLog> tLog_;
    
  };

};
