#include <TConfigs.h>
#include <TLog.h>

namespace liten
{

std::shared_ptr<TConfigs> TConfigs::tConfigs_ = nullptr;

/// Get a singleton instance, if not present create one
std::shared_ptr<TConfigs> TConfigs::GetInstance()
{
  if (tConfigs_ == nullptr)
  {
    tConfigs_ = std::make_shared<TConfigs::MakeSharedEnabler>();
  }
  TLOG(INFO) << "Created a new TConfigs";
  return tConfigs_;
}

}
