#include <TLog.h>

using namespace liten;

std::shared_ptr<TLog> TLog::tLog_ = nullptr;

const google::LogSeverity TLog::Info = google::INFO;

std::shared_ptr<TLog> TLog::GetInstance()
{
  if (nullptr == tLog_)
  {
    tLog_ = std::make_shared<TLog::MakeSharedEnabler>();
  }
  return tLog_;
};
