#pragma once

#include <chrono>

// TBD make it thread safe

namespace liten
{
  class StopWatch
  {
  public:
    using clock_resolution = std::chrono::high_resolution_clock;
    void Start()
    {
      start_ = clock_resolution::now();
      running_= true;
    }
    void Stop()
    {
      if (running_)
	stop_ = clock_resolution::now();
      running_ = false;
    }

    int64_t ElapsedInMicroseconds()
    {
      auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(stop_-start_);
      return microseconds.count();
    }

    int64_t ElapsedInMilliseconds()
    {
      auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(stop_-start_);
      return milliseconds.count();
    }
    
  private:
    std::chrono::time_point<clock_resolution> start_;
    std::chrono::time_point<clock_resolution> stop_;
    bool running_;
  };
};
