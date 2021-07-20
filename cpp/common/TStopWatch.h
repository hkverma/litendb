#pragma once

#include <chrono>

// TBD make it thread safe

namespace liten
{
class TStopWatch
{
public:
  using clock_resolution = std::chrono::high_resolution_clock;

  /// For now default constructors are ok
  TStopWatch() { }
  ~TStopWatch() { }
    
  /// Start clock
  void Start()
  {
    start_ = clock_resolution::now();
    running_= true;
  }

  /// Stop clock
  void Stop()
  {
    if (running_)
      stop_ = clock_resolution::now();
    running_ = false;
  }

  /// Time in microseconds
  int64_t ElapsedInMicroseconds()
  {
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(stop_-start_);
    return microseconds.count();
  }

  /// Time in milliseconds
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
}
