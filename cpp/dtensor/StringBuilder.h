#pragma once

#include <sstream>

namespace tendb
{
  
  template <typename Head>
  void StringBuilderRecursive(std::ostream& stream, Head&& head) {
    stream << head;
  }

  template <typename Head, typename... Tail>
  void StringBuilderRecursive(std::ostream& stream, Head&& head, Tail&&... tail) {
    StringBuilderRecursive(stream, std::forward<Head>(head));
    StringBuilderRecursive(stream, std::forward<Tail>(tail)...);
  }

  template <typename... Args>
  std::string StringBuilder(Args&&... args) {
    std::ostringstream ss;
    StringBuilderRecursive(ss, std::forward<Args>(args)...);
    return ss.str();
  }


};
