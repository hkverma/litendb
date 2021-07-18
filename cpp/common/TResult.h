#pragma once
// Adapted from Arrow

#include <cstddef>
#include <new>
#include <string>

#include <TStatus.h>

namespace liten
{

template <typename>
struct EnsureTResult;
  
namespace internal
{
void DieWithMessage(const std::string& msg);
void InvalidValueOrDie(const liten::TStatus& st);
using std::launder;
};


/// A class for representing either a usable value, or an error.
///
/// A TResult object either contains a value of type `T` or a TStatus object
/// explaining why such a value is not present. The type `T` must be
/// copy-constructible and/or move-constructible.
///
/// The state of a TResult object may be determined by calling ok() or
/// status(). The ok() method returns true if the object contains a valid value.
/// The status() method returns the internal TStatus object. A TResult object
/// that contains a valid value will return an OK TStatus for a call to status().
///
/// A value of type `T` may be extracted from a TResult object through a call
/// to ValueOrDie(). This function should only be called if a call to ok()
/// returns true. Sample usage:
///
/// ```
///   liten::TResult<Foo> result = CalculateFoo();
///   if (result.ok()) {
///     Foo foo = result.ValueOrDie();
///     foo.DoSomethingCool();
///   } else {
///    TLOG(ERROR) << result.status();
///  }
/// ```
///
/// If `T` is a move-only type, like `std::unique_ptr<>`, then the value should
/// only be extracted after invoking `std::move()` on the TResult object.
/// Sample usage:
///
/// ```
///   liten::TResult<std::unique_ptr<Foo>> result = CalculateFoo();
///   if (result.ok()) {
///     std::unique_ptr<Foo> foo = std::move(result).ValueOrDie();
///     foo->DoSomethingCool();
///   } else {
///     TLOG(ERROR) << result.status();
///   }
/// ```
///
/// TResult is provided for the convenience of implementing functions that
/// return some value but may fail during execution. For instance, consider a
/// function with the following signature:
///
/// ```
///   liten::TStatus CalculateFoo(int *output);
/// ```
///
/// This function may instead be written as:
///
/// ```
///   liten::TResult<int> CalculateFoo();
/// ```
template <class T>
class TResult
{
  
  template <typename U>
  friend class TResult;

  static_assert(!std::is_same<T, TStatus>::value,
                "this assert indicates you have probably made a metaprogramming error");

 public:
  using ValueType = T;

  /// Constructs a TResult object that contains a non-OK status.
  ///
  /// This constructor is marked `explicit` to prevent attempts to `return {}`
  /// from a function with a return type of, for example,
  /// `TResult<std::vector<int>>`. While `return {}` seems like it would return
  /// an empty vector, it will actually invoke the default constructor of
  /// TResult.
  explicit TResult()  // NOLINT(runtime/explicit)
      : status_(TStatus::UnknownError("Uninitialized TResult<T>")) {}

  ~TResult() noexcept { Destroy(); }

  /// Constructs a TResult object with the given non-OK TStatus object. All
  /// calls to ValueOrDie() on this object will abort. The given `status` must
  /// not be an OK status, otherwise this constructor will abort.
  ///
  /// This constructor is not declared explicit so that a function with a return
  /// type of `TResult<T>` can return a TStatus object, and the status will be
  /// implicitly converted to the appropriate return type as a matter of
  /// convenience.
  ///
  /// \param status The non-OK TStatus object to initialize to.
  TResult(const TStatus& status)  // NOLINT(runtime/explicit)
      : status_(status) {
    if (LITEN_UNLIKELY(status.ok())) {
      internal::DieWithMessage(std::string("Constructed with a non-error status: ") +
                               status.ToString());
    }
  }

  /// Constructs a TResult object that contains `value`. The resulting object
  /// is considered to have an OK status. The wrapped element can be accessed
  /// with ValueOrDie().
  ///
  /// This constructor is made implicit so that a function with a return type of
  /// `TResult<T>` can return an object of type `U &&`, implicitly converting
  /// it to a `TResult<T>` object.
  ///
  /// Note that `T` must be implicitly constructible from `U`, and `U` must not
  /// be a (cv-qualified) TStatus or TStatus-reference type. Due to C++
  /// reference-collapsing rules and perfect-forwarding semantics, this
  /// constructor matches invocations that pass `value` either as a const
  /// reference or as an rvalue reference. Since TResult needs to work for both
  /// reference and rvalue-reference types, the constructor uses perfect
  /// forwarding to avoid invalidating arguments that were passed by reference.
  /// See http://thbecker.net/articles/rvalue_references/section_08.html for
  /// additional details.
  ///
  /// \param value The value to initialize to.
  template <typename U,
            typename E = typename std::enable_if<
                std::is_constructible<T, U>::value && std::is_convertible<U, T>::value &&
                !std::is_same<typename std::remove_reference<
                                  typename std::remove_cv<U>::type>::type,
                              TStatus>::value>::type>
  TResult(U&& value) noexcept {  // NOLINT(runtime/explicit)
    ConstructValue(std::forward<U>(value));
  }

  /// Constructs a TResult object that contains `value`. The resulting object
  /// is considered to have an OK status. The wrapped element can be accessed
  /// with ValueOrDie().
  ///
  /// This constructor is made implicit so that a function with a return type of
  /// `TResult<T>` can return an object of type `T`, implicitly converting
  /// it to a `TResult<T>` object.
  ///
  /// \param value The value to initialize to.
  // NOTE `TResult(U&& value)` above should be sufficient, but some compilers
  // fail matching it.
  TResult(T&& value) noexcept {  // NOLINT(runtime/explicit)
    ConstructValue(std::move(value));
  }

  /// Copy constructor.
  ///
  /// This constructor needs to be explicitly defined because the presence of
  /// the move-assignment operator deletes the default copy constructor. In such
  /// a scenario, since the deleted copy constructor has stricter binding rules
  /// than the templated copy constructor, the templated constructor cannot act
  /// as a copy constructor, and any attempt to copy-construct a `TResult`
  /// object results in a compilation error.
  ///
  /// \param other The value to copy from.
  TResult(const TResult& other) : status_(other.status_) {
    if (LITEN_LIKELY(status_.ok())) {
      ConstructValue(other.ValueUnsafe());
    }
  }

  /// Templatized constructor that constructs a `TResult<T>` from a const
  /// reference to a `TResult<U>`.
  ///
  /// `T` must be implicitly constructible from `const U &`.
  ///
  /// \param other The value to copy from.
  template <typename U, typename E = typename std::enable_if<
                            std::is_constructible<T, const U&>::value &&
                            std::is_convertible<U, T>::value>::type>
  TResult(const TResult<U>& other) : status_(other.status_) {
    if (LITEN_LIKELY(status_.ok())) {
      ConstructValue(other.ValueUnsafe());
    }
  }

  /// Copy-assignment operator.
  ///
  /// \param other The TResult object to copy.
  TResult& operator=(const TResult& other) {
    // Check for self-assignment.
    if (this == &other) {
      return *this;
    }
    Destroy();
    status_ = other.status_;
    if (LITEN_LIKELY(status_.ok())) {
      ConstructValue(other.ValueUnsafe());
    }
    return *this;
  }

  /// Templatized constructor which constructs a `TResult<T>` by moving the
  /// contents of a `TResult<U>`. `T` must be implicitly constructible from `U
  /// &&`.
  ///
  /// Sets `other` to contain a non-OK status with a`TStatusError::Invalid`
  /// error code.
  ///
  /// \param other The TResult object to move from and set to a non-OK status.
  template <typename U,
            typename E = typename std::enable_if<std::is_constructible<T, U&&>::value &&
                                                 std::is_convertible<U, T>::value>::type>
  TResult(TResult<U>&& other) noexcept {
    if (LITEN_LIKELY(other.status_.ok())) {
      status_ = std::move(other.status_);
      ConstructValue(other.MoveValueUnsafe());
    } else {
      // If we moved the status, the other status may become ok but the other
      // value hasn't been constructed => crash on other destructor.
      status_ = other.status_;
    }
  }

  /// Move-assignment operator.
  ///
  /// Sets `other` to an invalid state..
  ///
  /// \param other The TResult object to assign from and set to a non-OK
  /// status.
  TResult& operator=(TResult&& other) noexcept {
    // Check for self-assignment.
    if (this == &other) {
      return *this;
    }
    Destroy();
    if (LITEN_LIKELY(other.status_.ok())) {
      status_ = std::move(other.status_);
      ConstructValue(other.MoveValueUnsafe());
    } else {
      // If we moved the status, the other status may become ok but the other
      // value hasn't been constructed => crash on other destructor.
      status_ = other.status_;
    }
    return *this;
  }

  /// Compare to another TResult.
  bool Equals(const TResult& other) const {
    if (LITEN_LIKELY(status_.ok())) {
      return other.status_.ok() && ValueUnsafe() == other.ValueUnsafe();
    }
    return status_.Equals(other.status_);
  }

  /// Indicates whether the object contains a `T` value.  Generally instead
  /// of accessing this directly you will want to use ASSIGN_OR_RAISE defined
  /// below.
  ///
  /// \return True if this TResult object's status is OK (i.e. a call to ok()
  /// returns true). If this function returns true, then it is safe to access
  /// the wrapped element through a call to ValueOrDie().
  bool ok() const { return status_.ok(); }

  /// \brief Equivalent to ok().
  // operator bool() const { return ok(); }

  /// Gets the stored status object, or an OK status if a `T` value is stored.
  ///
  /// \return The stored non-OK status object, or an OK status if this object
  ///         has a value.
  const TStatus& status() const { return status_; }

  /// Gets the stored `T` value.
  ///
  /// This method should only be called if this TResult object's status is OK
  /// (i.e. a call to ok() returns true), otherwise this call will abort.
  ///
  /// \return The stored `T` value.
  const T& ValueOrDie() const& {
    if (LITEN_UNLIKELY(!ok())) {
      internal::InvalidValueOrDie(status_);
    }
    return ValueUnsafe();
  }
  const T& operator*() const& { return ValueOrDie(); }
  const T* operator->() const { return &ValueOrDie(); }

  /// Gets a mutable reference to the stored `T` value.
  ///
  /// This method should only be called if this TResult object's status is OK
  /// (i.e. a call to ok() returns true), otherwise this call will abort.
  ///
  /// \return The stored `T` value.
  T& ValueOrDie() & {
    if (LITEN_UNLIKELY(!ok())) {
      internal::InvalidValueOrDie(status_);
    }
    return ValueUnsafe();
  }
  T& operator*() & { return ValueOrDie(); }
  T* operator->() { return &ValueOrDie(); }

  /// Moves and returns the internally-stored `T` value.
  ///
  /// This method should only be called if this TResult object's status is OK
  /// (i.e. a call to ok() returns true), otherwise this call will abort. The
  /// TResult object is invalidated after this call and will be updated to
  /// contain a non-OK status.
  ///
  /// \return The stored `T` value.
  T ValueOrDie() && {
    if (LITEN_UNLIKELY(!ok())) {
      internal::InvalidValueOrDie(status_);
    }
    return MoveValueUnsafe();
  }
  T operator*() && { return std::move(*this).ValueOrDie(); }

  /// Helper method for implementing TStatus returning functions in terms of semantically
  /// equivalent TResult returning functions. For example:
  ///
  /// TStatus GetInt(int *out) { return GetInt().Value(out); }
  template <typename U, typename E = typename std::enable_if<
                            std::is_constructible<U, T>::value>::type>
  TStatus Value(U* out) && {
    if (!ok()) {
      return status();
    }
    *out = U(MoveValueUnsafe());
    return TStatus::OK();
  }

  /// Move and return the internally stored value or alternative if an error is stored.
  T ValueOr(T alternative) && {
    if (!ok()) {
      return alternative;
    }
    return MoveValueUnsafe();
  }

  /// Retrieve the value if ok(), falling back to an alternative generated by the provided
  /// factory
  template <typename G>
  T ValueOrElse(G&& generate_alternative) && {
    if (ok()) {
      return MoveValueUnsafe();
    }
    return generate_alternative();
  }

  /// Apply a function to the internally stored value to produce a new result or propagate
  /// the stored error.
  template <typename M>
  typename EnsureTResult<typename std::result_of<M && (T)>::type>::type Map(M&& m) && {
    if (!ok()) {
      return status();
    }
    return std::forward<M>(m)(MoveValueUnsafe());
  }

  /// Apply a function to the internally stored value to produce a new result or propagate
  /// the stored error.
  template <typename M>
  typename EnsureTResult<typename std::result_of<M && (const T&)>::type>::type Map(
      M&& m) const& {
    if (!ok()) {
      return status();
    }
    return std::forward<M>(m)(ValueUnsafe());
  }

  /// Cast the internally stored value to produce a new result or propagate the stored
  /// error.
  template <typename U, typename E = typename std::enable_if<
                            std::is_constructible<U, T>::value>::type>
  TResult<U> As() && {
    if (!ok()) {
      return status();
    }
    return U(MoveValueUnsafe());
  }

  /// Cast the internally stored value to produce a new result or propagate the stored
  /// error.
  template <typename U, typename E = typename std::enable_if<
                            std::is_constructible<U, const T&>::value>::type>
  TResult<U> As() const& {
    if (!ok()) {
      return status();
    }
    return U(ValueUnsafe());
  }

  const T& ValueUnsafe() const& {
    return *internal::launder(reinterpret_cast<const T*>(&data_));
  }

  T& ValueUnsafe() & { return *internal::launder(reinterpret_cast<T*>(&data_)); }

  T ValueUnsafe() && { return MoveValueUnsafe(); }

  T MoveValueUnsafe() {
    return std::move(*internal::launder(reinterpret_cast<T*>(&data_)));
  }

 private:
  TStatus status_;  // pointer-sized
  typename std::aligned_storage<sizeof(T), alignof(T)>::type data_;

  template <typename U>
  void ConstructValue(U&& u) {
    new (&data_) T(std::forward<U>(u));
  }

  void Destroy() {
    if (LITEN_LIKELY(status_.ok())) {
      static_assert(offsetof(TResult<T>, status_) == 0,
                    "TStatus is guaranteed to be at the start of TResult<>");
      internal::launder(reinterpret_cast<const T*>(&data_))->~T();
    }
  }
};

#define ARROW_ASSIGN_OR_RAISE_IMPL(result_name, lhs, rexpr)                              \
  auto&& result_name = (rexpr);                                                          \
  ARROW_RETURN_IF_(!(result_name).ok(), (result_name).status(), ARROW_STRINGIFY(rexpr)); \
  lhs = std::move(result_name).ValueUnsafe();

#define ARROW_ASSIGN_OR_RAISE_NAME(x, y) ARROW_CONCAT(x, y)

/// \brief Execute an expression that returns a TResult, extracting its value
/// into the variable defined by `lhs` (or returning a TStatus on error).
///
/// Example: Assigning to a new value:
///   ARROW_ASSIGN_OR_RAISE(auto value, MaybeGetValue(arg));
///
/// Example: Assigning to an existing value:
///   ValueType value;
///   ARROW_ASSIGN_OR_RAISE(value, MaybeGetValue(arg));
///
/// WARNING: ARROW_ASSIGN_OR_RAISE expands into multiple statements;
/// it cannot be used in a single statement (e.g. as the body of an if
/// statement without {})!
///
/// WARNING: ARROW_ASSIGN_OR_RAISE `std::move`s its right operand. If you have
/// an lvalue TResult which you *don't* want to move out of cast appropriately.
#define ARROW_ASSIGN_OR_RAISE(lhs, rexpr)                                              \
  ARROW_ASSIGN_OR_RAISE_IMPL(ARROW_ASSIGN_OR_RAISE_NAME(_error_or_value, __COUNTER__), \
                             lhs, rexpr);

namespace internal {

template <typename T>
inline TStatus GenericToStatus(const TResult<T>& res) {
  return res.status();
}

template <typename T>
inline TStatus GenericToStatus(TResult<T>&& res) {
  return std::move(res).status();
}

}  // namespace internal

template <typename T>
TResult<T> ToTResult(T t) {
  return TResult<T>(std::move(t));
}

template <typename T>
struct EnsureTResult {
  using type = TResult<T>;
};

template <typename T>
struct EnsureTResult<TResult<T>> {
  using type = TResult<T>;
};

} // namespace liten

