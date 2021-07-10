//
// A Status encapsulates the result of an operation.  It may indicate success,
// or it may indicate an error with an associated error message.
//
// Multiple threads can invoke const methods on a Status without
// external synchronization, but if any of the threads may call a
// non-const method, all threads accessing the same Status must use
// external synchronization.

// Adapted from Apache Arrow, Apache Kudu, TensorFlow
#pragma once

#include <memory>
#include <Macros.h>
#include <StringBuilder.h>

/*
/// \brief Return with given status if condition is met.
#define LITEN_RETURN_IF_(condition, status, expr)       \
  do {                                                  \
    if (LITEN_PREDICT_FALSE(condition)) {               \
      ::liten::Status _st = (status);                   \
      _st.AddContextLine(__FILE__, __LINE__, expr);     \
      return _st;                                       \
    }                                                   \
  } while (0)

#define LITEN_RETURN_IF_(condition, status, _)  \
  do {                                          \
    if (LITEN_PREDICT_FALSE(condition)) {       \
      return (status);                          \
    }                                           \
  } while (0)

#endif  // LITEN_EXTRA_ERROR_CONTEXT

#define LITEN_RETURN_IF(condition, status)                      \
  LITEN_RETURN_IF_(condition, status, LITEN_STRINGIFY(status))

/// \brief Propagate any non-successful Status to the caller
#define LITEN_RETURN_NOT_OK(status)                                     \
  do {                                                                  \
    ::liten::Status __s = ::arrow::internal::GenericToStatus(status);   \
    LITEN_RETURN_IF_(!__s.ok(), __s, LITEN_STRINGIFY(status));          \
  } while (false)

#define RETURN_NOT_OK_ELSE(s, else_)                            \
  do {                                                          \
    ::arrow::Status _s = ::arrow::internal::GenericToStatus(s); \
    if (!_s.ok()) {                                             \
      else_;                                                    \
      return _s;                                                \
    }                                                           \
  } while (false)

// This is an internal-use macro and should not be used in public headers.
#ifndef RETURN_NOT_OK
#define RETURN_NOT_OK(s) LITEN_RETURN_NOT_OK(s)
#endif
*/
namespace liten {

  enum class StatusCode : char {
    OK = 0,
    OutOfMemory = 1,
    KeyError = 2,
    TypeError = 3,
    Invalid = 4,
    IOError = 5,
    CapacityError = 6,
    UnknownError = 7,
    NotImplemented = 8,
    IndexError = 9,
    SerializationError=10
  };

  /// \brief An abstract class that allows subsystems to add 
  /// additional information inside the Status
  class StatusDetail {
  public:
    virtual ~StatusDetail() = default;
    /// \brief Return a unique id for the type of the StatusDetail
    virtual const char* TypeId() const = 0;
    /// \brief Produce a human-readable description of this status
    virtual std::string ToString() const = 0;
    /// Same status if same Id and error string
    bool operator==(const StatusDetail& other) const noexcept
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
  class Status  {
  public:
    // \brief By default create a success status.
    Status() noexcept : state_(nullptr) {}
    ~Status() noexcept {
      // On certain compilers, splitting off the slow path improves
      // performance significantly.
      if (LITEN_PREDICT_FALSE(state_ != NULL)) {
        DeleteState();
      }
    }

    // \brief Create a status with msg
    Status(StatusCode code, const std::string& msg);
    
    /// \brief Pluggable constructor for use by sub-systems.  detail cannot be null.
    Status(StatusCode code, std::string msg, std::shared_ptr<StatusDetail> detail);

    // Copy the specified status.
    inline Status(const Status& s);
    inline Status& operator=(const Status& s);

    // Move the specified status.
    inline Status(Status&& s) noexcept;
    inline Status& operator=(Status&& s) noexcept;

    inline bool Equals(const Status& s) const;

    // AND the statuses.
    inline Status operator&(const Status& s) const noexcept;
    inline Status operator&(Status&& s) const noexcept;
    inline Status& operator&=(const Status& s) noexcept;
    inline Status& operator&=(Status&& s) noexcept;

    /// Return a success status
    static Status OK() { return Status(); }

    /// Status from given string arguments
    template <typename... Args>
    static Status FromArgs(StatusCode code, Args&&... args) {
      return Status(code, liten::StringBuilder(std::forward<Args>(args)...));
    }

    /// Status from given detail StatusDetail and string arguments    
    template <typename... Args>
    static Status FromDetailAndArgs(StatusCode code, std::shared_ptr<StatusDetail> detail,
                                    Args&&... args) {
      return Status(code, liten::StringBuilder(std::forward<Args>(args)...),
                    std::move(detail));
    }

    /// Return an error status for out-of-memory conditions
    template <typename... Args>
    static Status OutOfMemory(Args&&... args) {
      return Status::FromArgs(StatusCode::OutOfMemory, std::forward<Args>(args)...);
    }

    /// Return an error status for failed key lookups (e.g. column name in a table)
    template <typename... Args>
    static Status KeyError(Args&&... args) {
      return Status::FromArgs(StatusCode::KeyError, std::forward<Args>(args)...);
    }

    /// Return an error status for type errors (such as mismatching data types)
    template <typename... Args>
    static Status TypeError(Args&&... args) {
      return Status::FromArgs(StatusCode::TypeError, std::forward<Args>(args)...);
    }

    /// Return an error status for unknown errors
    template <typename... Args>
    static Status UnknownError(Args&&... args) {
      return Status::FromArgs(StatusCode::UnknownError, std::forward<Args>(args)...);
    }

    /// Return an error status when an operation or a combination of operation and
    /// data types is unimplemented
    template <typename... Args>
    static Status NotImplemented(Args&&... args) {
      return Status::FromArgs(StatusCode::NotImplemented, std::forward<Args>(args)...);
    }

    /// Return an error status for invalid data (for example a string that fails parsing)
    template <typename... Args>
    static Status Invalid(Args&&... args) {
      return Status::FromArgs(StatusCode::Invalid, std::forward<Args>(args)...);
    }

    /// Return an error status when an index is out of bounds
    template <typename... Args>
    static Status IndexError(Args&&... args) {
      return Status::FromArgs(StatusCode::IndexError, std::forward<Args>(args)...);
    }

    /// Return an error status when a container's capacity would exceed its limits
    template <typename... Args>
    static Status CapacityError(Args&&... args) {
      return Status::FromArgs(StatusCode::CapacityError, std::forward<Args>(args)...);
    }

    /// Return an error status when some IO-related operation failed
    template <typename... Args>
    static Status IOError(Args&&... args) {
      return Status::FromArgs(StatusCode::IOError, std::forward<Args>(args)...);
    }

    /// Return an error status when some (de)serialization operation failed
    template <typename... Args>
    static Status SerializationError(Args&&... args) {
      return Status::FromArgs(StatusCode::SerializationError, std::forward<Args>(args)...);
    }

    /// Return true iff the status indicates success.
    bool ok() const { return (state_ == nullptr); }

    /// Return true iff the status indicates an out-of-memory error.
    bool IsOutOfMemory() const { return code() == StatusCode::OutOfMemory; }
    /// Return true iff the status indicates a key lookup error.
    bool IsKeyError() const { return code() == StatusCode::KeyError; }
    /// Return true iff the status indicates invalid data.
    bool IsInvalid() const { return code() == StatusCode::Invalid; }
    /// Return true iff the status indicates an IO-related failure.
    bool IsIOError() const { return code() == StatusCode::IOError; }
    /// Return true iff the status indicates a container reaching capacity limits.
    bool IsCapacityError() const { return code() == StatusCode::CapacityError; }
    /// Return true iff the status indicates an out of bounds index.
    bool IsIndexError() const { return code() == StatusCode::IndexError; }
    /// Return true iff the status indicates a type error.
    bool IsTypeError() const { return code() == StatusCode::TypeError; }
    /// Return true iff the status indicates an unknown error.
    bool IsUnknownError() const { return code() == StatusCode::UnknownError; }
    /// Return true iff the status indicates an unimplemented operation.
    bool IsNotImplemented() const { return code() == StatusCode::NotImplemented; }
    /// Return true iff the status indicates a (de)serialization failure
    bool IsSerializationError() const { return code() == StatusCode::SerializationError; }

    /// \brief Return a string representation of this status suitable for printing.
    ///
    /// The string "OK" is returned for success.
    std::string ToString() const;

    /// \brief Return a string representation of the status code, without the message
    /// text or POSIX code information.
    std::string CodeAsString() const;
    static std::string CodeAsString(StatusCode);

    /// \brief Return the StatusCode value attached to this status.
    StatusCode code() const { return ok() ? StatusCode::OK : state_->code; }

    /// \brief Return the specific error message attached to this status.
    std::string message() const { return ok() ? "" : state_->msg; }

    /// \brief Return the status detail attached to this message.
    std::shared_ptr<StatusDetail> detail() const {
      return state_ == nullptr ? nullptr : state_->detail;
    }

    /// \brief Return a new Status copying the existing status, but
    /// updating with the existing detail.
    Status WithDetail(std::shared_ptr<StatusDetail> new_detail) const {
      return Status(code(), message(), std::move(new_detail));
    }

    /// \brief Return a new Status with changed message, copying the
    /// existing status code and detail.
    template <typename... Args>
    Status WithMessage(Args&&... args) const {
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
      StatusCode code;
      std::string msg;
      std::shared_ptr<StatusDetail> detail;
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
    void CopyFrom(const Status& s);

    /// Move to this status
    inline void MoveFrom(Status& s);
  };

  inline void Status::MoveFrom(Status& s) {
    delete state_;
    state_ = s.state_;
    s.state_ = nullptr;
  }

  Status::Status(const Status& s)
    : state_((s.state_ == nullptr) ? nullptr : new State(*s.state_)) {}

  Status& Status::operator=(const Status& s) {
    // The following condition catches both aliasing (when this == &s),
    // and the common case where both s and *this are ok.
    if (state_ != s.state_) {
      CopyFrom(s);
    }
    return *this;
  }

  Status::Status(Status&& s) noexcept : state_(s.state_) { s.state_ = nullptr; }

  Status& Status::operator=(Status&& s) noexcept {
    MoveFrom(s);
    return *this;
  }

  bool Status::Equals(const Status& s) const {
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
  Status Status::operator&(const Status& s) const noexcept {
    if (ok()) {
      return s;
    } else {
      return *this;
    }
  }

  Status Status::operator&(Status&& s) const noexcept {
    if (ok()) {
      return std::move(s);
    } else {
      return *this;
    }
  }

  Status& Status::operator&=(const Status& s) noexcept {
    if (ok() && !s.ok()) {
      CopyFrom(s);
    }
    return *this;
  }

  Status& Status::operator&=(Status&& s) noexcept {
    if (ok() && !s.ok()) {
      MoveFrom(s);
    }
    return *this;
  }
  /// \endcond

  namespace internal {

    // Extract Status from Status or Result<T>
    // Useful for the status check macros such as RETURN_NOT_OK.
    inline Status GenericToStatus(const Status& st) { return st; }
    inline Status GenericToStatus(Status&& st) { return std::move(st); }

  }  // namespace internal

}  // namespace liten
