#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>

namespace liten
{

/// Threadsafe UUID Generator using Boost UUID
class TGuid
{
public:

  using Uuid = ::boost::uuids::uuid;

  /// It is a singleton class
  static std::shared_ptr<TGuid> GetInstance();

  /// Create a new UUID
  inline Uuid GenerateUuid()
  {
    return idGenerator_();
  }

  ~TGuid() { }

private:

  /// Cannot be constructed
  TGuid() { }

  /// static singleton instance
  static std::shared_ptr<TGuid> tGuid_;

  /// Create an UUID random
  boost::uuids::random_generator idGenerator_;

  /// Allow shared_ptr with private constructors
  struct MakeSharedEnabler;

};

struct TGuid::MakeSharedEnabler : public TGuid {
  MakeSharedEnabler() : TGuid() { }
};


inline std::shared_ptr<TGuid> TGuid::GetInstance()
{
  if (nullptr == tGuid_)
  {
    tGuid_ = std::make_shared<MakeSharedEnabler>();
  }
  return tGuid_;
};
  
};

