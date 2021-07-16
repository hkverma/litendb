#include <TTimeUtils.h>

namespace liten
{

  // for now use boost::posix_time
  // Use time_point calendars when move to C++-20 standards
  int64_t SecondsSinceEpoch(boost::gregorian::date d,
                            boost::posix_time::time_duration offset)
  {
    boost::posix_time::ptime t(d, offset);
    boost::posix_time::ptime start(boost::gregorian::date(1970,1,1));
    boost::posix_time::time_duration dur = t - start;
    time_t epoch = dur.total_seconds();
    return (int64_t)epoch;
  }

  // ISO 86101 string "2002-01-20 23:59:59"
  int64_t SecondsSinceEpoch(std::string& ts)
  {
    boost::posix_time::ptime t(boost::posix_time::time_from_string(ts));
    boost::posix_time::ptime start(boost::gregorian::date(1970,1,1));
    boost::posix_time::time_duration dur = t - start;
    time_t epoch = dur.total_seconds();
    return (int64_t)epoch;
  }

  int32_t DaysSinceEpoch(boost::gregorian::date d,
                         boost::posix_time::time_duration offset)
  {
    int64_t dur = SecondsSinceEpoch(d, offset);
    dur = dur/(24*60*60);
    int32_t days = (int32_t)dur;
    return days;
  }
  
};
