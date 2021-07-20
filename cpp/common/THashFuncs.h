#pragma once

#include <boost/functional/hash.hpp>

namespace liten {
  
// A hash function used to hash a pair of any kind
struct hash_pair {
  template <class T1, class T2>
  size_t operator()(const std::pair<T1, T2>& p) const
  {
    auto hash1 = std::hash<T1>{}(p.first);
    auto hash2 = std::hash<T2>{}(p.second);
    return hash1 ^ hash2;
  }
};

// Use boost hash function e.g. for uuid
struct hash_boost {
  template<class T>
  size_t operator () (const T& p1) const
  {
    return boost::hash<T>()(p1);
  }
};
  
};
