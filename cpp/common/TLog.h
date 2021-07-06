#pragma once

#include <memory>
#include <glog/logging.h>

/// Use macros as is from google for now, TLog must have been initialized
#define TLOG(severity) COMPACT_GOOGLE_LOG_ ## severity.stream()

namespace liten
{
  
  /// Liten wide logging use this interface, using google logging for now
  class TLog
  {

  public:

    /// Get Singleton Instance
    static std::shared_ptr<TLog> GetInstance()
    {
      if (nullptr == tLog_)
      {
        tLog_ = std::make_shared<TLog>();
      }
      return tLog_;
    };

    /// Get Singleton Instance to start logging
    std::shared_ptr<TLog> Start()
    {
      return GetInstance();
    };

    // Stop all logging
    void Stop()
    {
      // Logging never started
      if (nullptr == tLog_)
      {
        return ;
      }
      TLOG(INFO) << "Stop Liten Logger" ;
      google::ShutdownGoogleLogging();
    };

    /// Initialize google logging at construction
    TLog()
    {
      google::InitGoogleLogging("Liten");
      TLOG(INFO) << "Start Liten Logger" ;
    };
    
    ~TLog() { }
    
  private:


    /// Singleton log class
    static std::shared_ptr<TLog> tLog_;
    
  };
  
};

