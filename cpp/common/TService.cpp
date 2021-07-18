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
    
}
