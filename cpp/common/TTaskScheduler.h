#pragma once

#include <oneapi/tbb.h>

namespace liten
{

/// Schedules tasks across multiple threads
class TTaskScheduler {
public:
    
  /// Create a task scheduler. Should be started by TService.
  TTaskScheduler();

  /// Execute task on arena arenaId
  void Execute(std::function<void()> task, int64_t arenaId);

private:

  std::vector<tbb::numa_node_id> numaIndexes_;
  std::unique_ptr<std::vector<tbb::task_arena>> taskArenas_;
  std::unique_ptr<std::vector<tbb::task_group>> taskGroups_;

};

};
