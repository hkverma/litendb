#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>

namespace liten
{
int64_t SecondsSinceEpoch(boost::gregorian::date d,
                          boost::posix_time::time_duration offset=boost::posix_time::seconds(0));
int64_t SecondsSinceEpoch(std::string& ts);
int32_t DaysSinceEpoch(boost::gregorian::date d,
                       boost::posix_time::time_duration offset=boost::posix_time::seconds(0));
  
}
