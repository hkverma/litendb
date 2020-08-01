#include <chrono>

#pragma once
namespace tendb
{
  class StopWatch
  {
  public:
    using clock_resolution = std::chrono::high_resolution_clock;
    void Start()
    {
      start_ = clock_resolution::now();
    }
    void Stop()
    {
      stop_ = clock_resolution::now();
    }

    int64_t ElapsedInMicroseconds()
    {
      auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(stop_-start_);
      return microseconds.count();
    }
      
  private:
    std::chrono::time_point<clock_resolution> start_;
    std::chrono::time_point<clock_resolution> stop_;
  };
};
