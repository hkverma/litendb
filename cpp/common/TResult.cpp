#include <string>

#include <TResult.h>
#include <TLog.h>

namespace liten
{

namespace internal
{

void DieWithMessage(const std::string& msg)
{
  TLOG(FATAL) << msg;
}

void InvalidValueOrDie(const TStatus& st) {
  DieWithMessage(std::string("ValueOrDie called on an error: ") + st.ToString());
}

}
}
