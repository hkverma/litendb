#include <TLog.h>

using namespace liten;

std::shared_ptr<TLog> TLog::tLog_ = nullptr;

const google::LogSeverity TLog::Info = google::INFO;
