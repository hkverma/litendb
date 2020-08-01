#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>

namespace tendb
{
  int64_t SecondsSinceEpoch(boost::gregorian::date d, boost::posix_time::time_duration td);
  int64_t SecondsSinceEpoch(std::string& ts);
}
