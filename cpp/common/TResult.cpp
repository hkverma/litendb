#include <string>

#include <TResult.h>
#include <TLog.h>

namespace liten
{

namespace internal
{

// Fatal log terminates the program after logging the message
void DieWithMessage(const std::string& msg)
{
  TLOG(FATAL) << msg;
}

void InvalidValueOrDie(const TStatus& st) {
  DieWithMessage(std::string("ValueOrDie called on an error: ") + st.ToString());
}

}
}
