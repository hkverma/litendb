#include <TTaskScheduler.h>
#include <TLog.h>

namespace liten
{

TTaskScheduler::TTaskScheduler()
{
  numaIndexes_ = tbb::info::numa_nodes();
  taskArenas_ = std::make_unique<std::vector<tbb::task_arena>>(numaIndexes_.size());
  taskGroups_ = std::make_unique<std::vector<tbb::task_group>>(numaIndexes_.size());  
  TLOG(INFO) << "Discovered " << numaIndexes_.size() << " NUMA nodes.";
  for(unsigned numaId = 0; numaId < numaIndexes_.size(); numaId++) {
    taskArenas_->at(numaId).initialize(tbb::task_arena::constraints(numaIndexes_[numaId]));
    TLOG(INFO) << "Created arena and task group for NUMA node=" << numaId << " with concurrency=" << tbb::detail::d1::default_concurrency(numaId);
  }
}

void TTaskScheduler::Execute(std::function<void()> task, int64_t arenaId)
{
  auto& tg = taskGroups_->at(arenaId);
  auto execTask = [&tg, &task]() {
    tg.run(task);
  };
  taskArenas_->at(arenaId).execute(execTask);
}

};

