#include <chrono>

#pragma once
namespace tendb
{
  class TStopWatch
  {
  public:
    void Start()
    {
      start_ = std::chrono::high_resolution_clock::now();
    }
    void Stop()
    {
      stop_ = std::chrono::high_resolution_clock::now();
    }

    int64_t ElapsedInMicroseconds()
    {
      auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(stop_-start_);
      return microseconds.count();
    }
      
  private:
    std::chrono::time_point start_;
    std::chrono::time_point finish_;
  };
};
