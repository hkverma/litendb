#include <tbb/tbb.h>
#include <TService.h>

namespace liten
{

std::shared_ptr<TService> TService::tService_ = nullptr;

std::shared_ptr<TService> TService::GetInstance()
{
  if (nullptr == tService_)
  {
    tService_ = std::make_shared<TService::MakeSharedEnabler>();
  }
  return tService_;
}

void TService::Start()
{
  tLog_ = TLog::GetInstance()->Start();
  TLOG(INFO) << "Start Liten Services";
};

void TService::Shutdown()
{
  TLOG(INFO) << "Stop Liten Services";
  tLog_->Stop();
}
    
}
