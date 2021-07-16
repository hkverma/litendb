#pragma once

// TBD wrap TBB here for a task scheduler
// TBD write an assert library
// TBD Change Status name to TStatus to avoid confusion with other Status
// TBD Clean for all constness

// TBD write result.h

#include <vector>
#include <string>
#include <shared_mutex>
#include <mutex>
#include <memory>
#include <iostream>
#include <set>
#include <sstream>
#include <unordered_map>

#include <cstdint>
#include <tbb/tbb.h>

#include <arrow/api.h>
#include <arrow/csv/api.h>
#include <arrow/filesystem/api.h>
#include <arrow/io/api.h>
#include <arrow/util/string_view.h>

#include <THashFuncs.h>
#include <TMacros.h>
#include <TTimeUtils.h>
#include <TStopWatch.h>
#include <TStringBuilder.h>
#include <TStatus.h>
#include <TLog.h>
#include <TService.h>
#include <TGuid.h>
#include <TConfigs.h>
