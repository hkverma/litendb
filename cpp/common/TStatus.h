#pragma once

#include <memory>

#include <TMacros.h>
#include <TStringBuilder.h>

//
// A Status encapsulates the result of an operation.  It may indicate success,
// or it may indicate an error with an associated error message.
//
// Multiple threads can invoke const methods on a Status without
// external synchronization, but if any of the threads may call a
// non-const method, all threads accessing the same Status must use
// external synchronization.

// Adapted from Apache Arrow, Apache Kudu, TensorFlow
//
//

#ifdef LITEN_EXTRA_ERROR_CONTEXT

/// \brief Return with given status if condition is met.
#define LITEN_RETURN_IF_(condition, status, expr)   \
do                                                    \
{                                                     \
  if (LITEN_UNLIKELY(condition))                      \
  {                                                   \
    ::liten::Status _st = (status);                   \
    _st.AddContextLine(__FILE__, __LINE__, expr);     \
    return _st;                                       \
  }                                                   \
} while (0)

#else

#define LITEN_RETURN_IF_(condition, status, _)   \
do                                                 \
{                                                  \
  if (LITEN_UNLIKELY(condition)) {                 \
    return (status);                               \
  }                                                \
} while (0)

#endif  // LITEN_EXTRA_ERROR_CONTEXT


#define LITEN_RETURN_IF(condition, status)                      \
LITEN_RETURN_IF_(condition, status, LITEN_STRINGIFY(status))

/// \brief Propagate any non-successful Status to the caller
#define LITEN_RETURN_NOT_OK(status)                             \
do                                                                \
{                                                                 \
  ::liten::TStatus __s = ::liten::GenericToTStatus(status);        \
    ARROW_RETURN_IF_(!__s.ok(), __s, ARROW_STRINGIFY(status));    \
} while (false)
 
#define LITEN_RETURN_NOT_OK_ELSE(s, else_)                 \
do {                                                          \
  ::liten::TStatus _s = ::liten::GenericToTStatus(s);          \
  if (!_s.ok()) {                                             \
   else_;                                                    \
   return _s;                                                \
  }                                                           \
} while (false)

namespace liten {

enum class TStatusCode : char {
  OK = 0,
  // Genetic Codes
  OutOfMemory = 1,
  KeyError = 2,
  TypeError = 3,
  Invalid = 4,
  IOError = 5,
  CapacityError = 6,
  UnknownError = 7,
  NotImplemented = 8,
  IndexError = 9,
  SerializationError = 10,
  AlreadyExists = 11
};

/// \brief An abstract class that allows subsystems to add
/// additional information inside the Status
class TStatusDetail {
public:
  virtual ~TStatusDetail() = default;
  /// \brief Return a unique id for the type of the StatusDetail
  virtual const char* TypeId() const = 0;
  /// \brief Produce a human-readable description of this status
  virtual std::string ToString() const = 0;
  /// Same status if same Id and error string
  bool operator==(const TStatusDetail& other) const noexcept
  {
    return std::string(TypeId()) == other.TypeId() && ToString() == other.ToString();
  }
};

/// \brief Status outcome object (success or error)
///
/// The Status object is an object holding the outcome of an operation.
/// The outcome is represented as a StatusCode, either success
/// (StatusCode::OK) or an error (any other of the StatusCode enumeration values).
///
/// Additionally, if an error occurred, a specific error message is generally
/// attached.
class TStatus  {
public:
  // \brief By default create a success status.
  TStatus() noexcept : state_(nullptr) {}
  ~TStatus() noexcept {
    // On certain compilers, splitting off the slow path improves
    // performance significantly.
    if (LITEN_UNLIKELY(state_ != NULL)) {
      DeleteState();
    }
  }

  // \brief Create a status with msg
  TStatus(TStatusCode code, const std::string& msg);

  /// \brief Pluggable constructor for use by sub-systems.  detail cannot be null.
  TStatus(TStatusCode code, std::string msg, std::shared_ptr<TStatusDetail> detail);

  // Copy the specified status.
  inline TStatus(const TStatus& s);
  inline TStatus& operator=(const TStatus& s);

  // Move the specified status.
  inline TStatus(TStatus&& s) noexcept;
  inline TStatus& operator=(TStatus&& s) noexcept;

  inline bool Equals(const TStatus& s) const;

  // AND the statuses.
  inline TStatus operator&(const TStatus& s) const noexcept;
  inline TStatus operator&(TStatus&& s) const noexcept;
  inline TStatus& operator&=(const TStatus& s) noexcept;
  inline TStatus& operator&=(TStatus&& s) noexcept;

  /// Return a success status
  static TStatus OK() { return TStatus(); }

  /// TStatus from given string arguments
  template <typename... Args>
  static TStatus FromArgs(TStatusCode code, Args&&... args) {
    return TStatus(code, liten::StringBuilder(std::forward<Args>(args)...));
  }

  /// TStatus from given detail TStatusDetail and string arguments
  template <typename... Args>
  static TStatus FromDetailAndArgs(TStatusCode code, std::shared_ptr<TStatusDetail> detail,
                                   Args&&... args) {
    return TStatus(code, liten::StringBuilder(std::forward<Args>(args)...),
                   std::move(detail));
  }

  /// Return an error status for out-of-memory conditions
  template <typename... Args>
  static TStatus OutOfMemory(Args&&... args) {
    return TStatus::FromArgs(TStatusCode::OutOfMemory, std::forward<Args>(args)...);
  }

  /// Return an error status for failed key lookups (e.g. column name in a table)
  template <typename... Args>
  static TStatus KeyError(Args&&... args) {
    return TStatus::FromArgs(TStatusCode::KeyError, std::forward<Args>(args)...);
  }

  /// Return an error status for type errors (such as mismatching data types)
  template <typename... Args>
  static TStatus TypeError(Args&&... args) {
    return TStatus::FromArgs(TStatusCode::TypeError, std::forward<Args>(args)...);
  }

  /// Return an error status for unknown errors
  template <typename... Args>
  static TStatus UnknownError(Args&&... args) {
    return TStatus::FromArgs(TStatusCode::UnknownError, std::forward<Args>(args)...);
  }

  /// Return an error status when an operation or a combination of operation and
  /// data types is unimplemented
  template <typename... Args>
  static TStatus NotImplemented(Args&&... args) {
    return TStatus::FromArgs(TStatusCode::NotImplemented, std::forward<Args>(args)...);
  }

  /// Return an error status for invalid data (for example a string that fails parsing)
  template <typename... Args>
  static TStatus Invalid(Args&&... args) {
    return TStatus::FromArgs(TStatusCode::Invalid, std::forward<Args>(args)...);
  }

  /// Return an error status when an index is out of bounds
  template <typename... Args>
  static TStatus IndexError(Args&&... args) {
    return TStatus::FromArgs(TStatusCode::IndexError, std::forward<Args>(args)...);
  }

  /// Return an error status when a container's capacity would exceed its limits
  template <typename... Args>
  static TStatus CapacityError(Args&&... args) {
    return TStatus::FromArgs(TStatusCode::CapacityError, std::forward<Args>(args)...);
  }

  /// Return an error status when some IO-related operation failed
  template <typename... Args>
  static TStatus IOError(Args&&... args) {
    return TStatus::FromArgs(TStatusCode::IOError, std::forward<Args>(args)...);
  }

  /// Return an error status when some (de)serialization operation failed
  template <typename... Args>
  static TStatus SerializationError(Args&&... args) {
    return TStatus::FromArgs(TStatusCode::SerializationError, std::forward<Args>(args)...);
  }

  /// Return an error status when something already exists
  template <typename... Args>
  static TStatus AlreadyExists(Args&&... args) {
    return TStatus::FromArgs(TStatusCode::AlreadyExists, std::forward<Args>(args)...);
  }

  /// Return true iff the status indicates success.
  bool ok() const { return (state_ == nullptr); }

  /// Return true iff the status indicates an out-of-memory error.
  bool IsOutOfMemory() const { return code() == TStatusCode::OutOfMemory; }
  /// Return true iff the status indicates a key lookup error.
  bool IsKeyError() const { return code() == TStatusCode::KeyError; }
  /// Return true iff the status indicates invalid data.
  bool IsInvalid() const { return code() == TStatusCode::Invalid; }
  /// Return true iff the status indicates an IO-related failure.
  bool IsIOError() const { return code() == TStatusCode::IOError; }
  /// Return true iff the status indicates a container reaching capacity limits.
  bool IsCapacityError() const { return code() == TStatusCode::CapacityError; }
  /// Return true iff the status indicates an out of bounds index.
  bool IsIndexError() const { return code() == TStatusCode::IndexError; }
  /// Return true iff the status indicates a type error.
  bool IsTypeError() const { return code() == TStatusCode::TypeError; }
  /// Return true iff the status indicates an unknown error.
  bool IsUnknownError() const { return code() == TStatusCode::UnknownError; }
  /// Return true iff the status indicates an unimplemented operation.
  bool IsNotImplemented() const { return code() == TStatusCode::NotImplemented; }
  /// Return true iff the status indicates a (de)serialization failure
  bool IsSerializationError() const { return code() == TStatusCode::SerializationError; }

  /// \brief Return a string representation of this status suitable for printing.
  ///
  /// The string "OK" is returned for success.
  std::string ToString() const;

  /// \brief Return a string representation of the status code, without the message
  /// text or POSIX code information.
  std::string CodeAsString() const;
  static std::string CodeAsString(TStatusCode);

  /// \brief Return the TStatusCode value attached to this status.
  TStatusCode code() const { return ok() ? TStatusCode::OK : state_->code; }

  /// \brief Return the specific error message attached to this status.
  std::string message() const { return ok() ? "" : state_->msg; }

  /// \brief Return the status detail attached to this message.
  std::shared_ptr<TStatusDetail> detail() const {
    return state_ == nullptr ? nullptr : state_->detail;
  }

  /// \brief Return a new TStatus copying the existing status, but
  /// updating with the existing detail.
  TStatus WithDetail(std::shared_ptr<TStatusDetail> new_detail) const {
    return TStatus(code(), message(), std::move(new_detail));
  }

  /// \brief Return a new TStatus with changed message, copying the
  /// existing status code and detail.
  template <typename... Args>
  TStatus WithMessage(Args&&... args) const {
    return FromArgs(code(), std::forward<Args>(args)...).WithDetail(detail());
  }

  [[noreturn]] void Abort() const;
  [[noreturn]] void Abort(const std::string& message) const;

#ifdef LITEN_EXTRA_ERROR_CONTEXT
  void AddContextLine(const char* filename, int line, const char* expr);
#endif

private:

  /// State is used if there is a non-OK status
  struct State {
    TStatusCode code;
    std::string msg;
    std::shared_ptr<TStatusDetail> detail;
  };

  /// OK status has a `NULL` state_.  Otherwise, `state_` points to
  /// a `State` structure containing the error code and message(s)
  State* state_;

  // Delete the state of status
  void DeleteState() {
    delete state_;
    state_ = nullptr;
  }

  /// Copy status to this status
  void CopyFrom(const TStatus& s);

  /// Move to this status
  inline void MoveFrom(TStatus& s);
};

inline void TStatus::MoveFrom(TStatus& s) {
  delete state_;
  state_ = s.state_;
  s.state_ = nullptr;
}

TStatus::TStatus(const TStatus& s)
  : state_((s.state_ == nullptr) ? nullptr : new State(*s.state_)) {}

TStatus& TStatus::operator=(const TStatus& s) {
  // The following condition catches both aliasing (when this == &s),
  // and the common case where both s and *this are ok.
  if (state_ != s.state_) {
    CopyFrom(s);
  }
  return *this;
}

TStatus::TStatus(TStatus&& s) noexcept : state_(s.state_) { s.state_ = nullptr; }

TStatus& TStatus::operator=(TStatus&& s) noexcept {
  MoveFrom(s);
  return *this;
}

bool TStatus::Equals(const TStatus& s) const {
  if (state_ == s.state_) {
    return true;
  }

  if (ok() || s.ok()) {
    return false;
  }

  if (detail() != s.detail()) {
    if ((detail() && !s.detail()) || (!detail() && s.detail())) {
      return false;
    }
    return *detail() == *s.detail();
  }

  return code() == s.code() && message() == s.message();
}

/// \cond FALSE
// (note: emits warnings on Doxygen < 1.8.15,
//  see https://github.com/doxygen/doxygen/issues/6295)
TStatus TStatus::operator&(const TStatus& s) const noexcept {
  if (ok()) {
    return s;
  } else {
    return *this;
  }
}

TStatus TStatus::operator&(TStatus&& s) const noexcept {
  if (ok()) {
    return std::move(s);
  } else {
    return *this;
  }
}

TStatus& TStatus::operator&=(const TStatus& s) noexcept {
  if (ok() && !s.ok()) {
    CopyFrom(s);
  }
  return *this;
}

TStatus& TStatus::operator&=(TStatus&& s) noexcept {
  if (ok() && !s.ok()) {
    MoveFrom(s);
  }
  return *this;
}
/// \endcond

// Extract Status from TStatus or TResult<T>
// Useful for the status check macros such as RETURN_NOT_OK.
inline TStatus GenericToTStatus(const TStatus& st) { return st; }
inline TStatus GenericToTStatus(TStatus&& st) { return std::move(st); }

}  // namespace liten
