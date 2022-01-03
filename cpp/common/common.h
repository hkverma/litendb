#pragma once

// TBD wrap TBB here for a task scheduler
// TBD write an assert library
// TBD Clean for all constness

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
#include <oneapi/tbb.h>

#include <arrow/api.h>
#include <arrow/csv/api.h>
#include <arrow/filesystem/api.h>
#include <arrow/io/api.h>
#include <arrow/util/string_view.h>

#include <THashFuncs.h>
#include <TJSon.h>
#include <TMacros.h>
#include <TTimeUtils.h>
#include <TFuncUtils.h>
#include <TStopWatch.h>
#include <TStringBuilder.h>
#include <TStatus.h>
#include <TTaskScheduler.h>
#include <TLog.h>
#include <TService.h>
#include <TGuid.h>
#include <TConfigs.h>
#include <TResult.h>
