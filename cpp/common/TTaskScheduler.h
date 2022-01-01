#pragma once

#include <oneapi/tbb.h>

namespace liten
{

/// Schedules tasks across multiple threads
class TTaskScheduler {
public:

  // On one node, there can only be one task scheduler
  static std::shared_ptr<TTaskScheduler> GetInstance();

  /// create arenas and task groups
  void Start();

  /// Finish all tasks from the task groups
  void Stop() { Wait(); };

  /// wait for taskgroups to complete their tasks
  void Wait(int64_t numaId);

  /// Wait for all tasks to complete
  void Wait();
  
  /// Execute task on arena numaId
  void Execute(std::function<void()> task, int64_t numaId);

  int64_t NumWorkerThreads() const { return numWorkerThreads_; }
  

private:

  /// Separate task groups for each NUMA Id
  std::vector<tbb::numa_node_id> numaIndexes_;
  std::unique_ptr<std::vector<tbb::task_arena>> taskArenas_;
  std::unique_ptr<std::vector<tbb::task_group>> taskGroups_;
  int64_t numWorkerThreads_ = 0;

  /// Singleton element
  static std::shared_ptr<TTaskScheduler> tTaskScheduler_;

  /// Create a task scheduler. Should be started by TTaskScheduler.
  TTaskScheduler() { }

  /// Allow shared_ptr with private constructors
  struct MakeSharedEnabler;  

};

struct TTaskScheduler::MakeSharedEnabler : public TTaskScheduler {
  MakeSharedEnabler() : TTaskScheduler() { }
};  

};
