#include <TTaskScheduler.h>
#include <TLog.h>

namespace liten
{

std::shared_ptr<TTaskScheduler> TTaskScheduler::tTaskScheduler_ = nullptr;

std::shared_ptr<TTaskScheduler> TTaskScheduler::GetInstance()
{
  if (nullptr == tTaskScheduler_)
  {
    tTaskScheduler_ = std::make_shared<TTaskScheduler::MakeSharedEnabler>();
  }
  return tTaskScheduler_;
}

void TTaskScheduler::Start()
{
  
  numWorkerThreads_ = 1;
  
  numaIndexes_ = tbb::info::numa_nodes();
  taskArenas_ = std::make_unique<std::vector<tbb::task_arena>>(numaIndexes_.size());
  taskGroups_ = std::make_unique<std::vector<tbb::task_group>>(numaIndexes_.size());  
  TLOG(INFO) << "Discovered " << numaIndexes_.size() << " NUMA nodes.";
  for(unsigned numaId = 0; numaId < numaIndexes_.size(); numaId++) {
    // Use default if needed tbb::detail::d1::default_concurrency(numaId)
    tbb::task_arena::constraints arenaConstraints(numaIndexes_[numaId]);
    arenaConstraints.set_max_concurrency(numWorkerThreads_);
    taskArenas_->at(numaId).initialize(arenaConstraints);
    TLOG(INFO) << "Created arena and task group for NUMA node=" << numaId << " with concurrency=" << arenaConstraints.max_concurrency;
  }
}

void TTaskScheduler::Wait()
{
  for(unsigned numaId = 0; numaId < numaIndexes_.size(); numaId++) {
    Wait(numaId);
  }
}

void TTaskScheduler::Wait(int64_t numaId)
{
  auto& tg = taskGroups_->at(numaId);
  taskArenas_->at(numaId).execute([&tg]() { tg.wait(); });
}
  
void TTaskScheduler::Execute(std::function<void()> task, int64_t numaId)
{
  auto& tg = taskGroups_->at(numaId);
  auto execTask = [&tg, &task]() {
    tg.run(task);
  };
  taskArenas_->at(numaId).execute(execTask);
}

};

