#pragma once

//
// Currently support only GNU CC

#if defined(__GNUC__)
/// Use likely or unlikely to help compiler with branch prediction
/// likely predicts true while unlikely predicts false
#define LITEN_UNLIKELY(x) (__builtin_expect(!!(x),0))
#define LITEN_LIKELY(x) (__builtin_expect(!!(x),1))
#define LITEN_NORETURN __attribute__((noreturn))
#define LITEN_PREFETCH(addr) __builtin_prefetch(addr)
#else
#define LITEN_NORETURN
#define LITEN_UNLIKELY(x) (x)
#define LITEN_LIKELY(x) (x)
#define LITEN_PREFETCH(addr)
#endif

#define LITEN_STRINGIFY(x) #x

#define LITEN_EXIT_ON_FAILURE(expr)              \
  do {                                             \
    liten::TStatus status_ = (expr);               \
    if (!status_.ok()) {                           \
      std::cerr << status_.message() << std::endl; \
      return EXIT_FAILURE;                         \
    }                                              \
  } while (0);

