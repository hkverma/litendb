// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// A Status encapsulates the result of an operation.  It may indicate success,
// or it may indicate an error with an associated error message.
//
// Multiple threads can invoke const methods on a Status without
// external synchronization, but if any of the threads may call a
// non-const method, all threads accessing the same Status must use
// external synchronization.

#include <glog/logging.h>
#include <TStatus.h>

namespace liten {

TStatus::TStatus(TStatusCode code, const std::string& msg)
  : TStatus::TStatus(code, msg, nullptr) {}

TStatus::TStatus(TStatusCode code, std::string msg, std::shared_ptr<TStatusDetail> detail)
{
  LOG_IF(INFO, (code == TStatusCode::OK)) << "Cannot construct ok status with message";
  state_ = new State;
  state_->code = code;
  state_->msg = std::move(msg);
  if (detail != nullptr)
  {
    state_->detail = std::move(detail);
  }
}

void TStatus::CopyFrom(const TStatus& s)
{
  delete state_;
  if (s.state_ == nullptr) {
    state_ = nullptr;
  } else {
    state_ = new State(*s.state_);
  }
}

std::string TStatus::CodeAsString() const {
  if (state_ == nullptr) {
    return "OK";
  }
  return CodeAsString(code());
}

std::string TStatus::CodeAsString(TStatusCode code) {
  const char* type;
  switch (code) {
  case TStatusCode::OK:
    type = "OK";
    break;
  case TStatusCode::OutOfMemory:
    type = "Out of memory";
    break;
  case TStatusCode::KeyError:
    type = "Key error";
    break;
  case TStatusCode::TypeError:
    type = "Type error";
    break;
  case TStatusCode::Invalid:
    type = "Invalid";
    break;
  case TStatusCode::IOError:
    type = "IOError";
    break;
  case TStatusCode::CapacityError:
    type = "Capacity error";
    break;
  default:
    type = "Unknown";
    break;
  }
  return std::string(type);
}

std::string TStatus::ToString() const {
  std::string result(CodeAsString());
  if (state_ == nullptr) {
    return result;
  }
  result += ": ";
  result += state_->msg;
  if (state_->detail != nullptr) {
    result += ". Detail: ";
    result += state_->detail->ToString();
  }

  return result;
}

}
