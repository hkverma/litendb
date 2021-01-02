#pragma once

//
// Currently support only GNU CC

#if defined(__GNUC__)
#define LITEN_PREDICT_FALSE(x) (__builtin_expect(!!(x), 0))
#define LITEN_PREDICT_TRUE(x) (__builtin_expect(!!(x), 1))
#define LITEN_NORETURN __attribute__((noreturn))
#define LITEN_PREFETCH(addr) __builtin_prefetch(addr)
#else
#define LITEN_NORETURN
#define LITEN_PREDICT_FALSE(x) (x)
#define LITEN_PREDICT_TRUE(x) (x)
#define LITEN_PREFETCH(addr)
#endif

#define EXIT_ON_FAILURE(expr)                      \
  do {                                             \
    arrow::Status status_ = (expr);                \
    if (!status_.ok()) {                           \
      std::cerr << status_.message() << std::endl; \
      return EXIT_FAILURE;                         \
    }                                              \
  } while (0);

