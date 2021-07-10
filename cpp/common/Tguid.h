#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>

/// provide hash template with uuid
namespace std
{
  template<>
  struct hash<boost::uuids::uuid>
  {
    size_t operator () (const boost::uuids::uuid& uid) const
    {
        return boost::hash<boost::uuids::uuid>()(uid);
    }
  };
  
};

namespace liten
{

  /// Threadsafe UUID Generator using Boost UUID
  class Tguid
  {
  public:

    using Uuid = boost::uuids::uuid;

    /// It is a singleton class
    static std::shared_ptr<Tguid> GetInstance();

    /// Create a new UUID
    inline Uuid GenerateUuid()
    {
      return idGenerator_();
    }

    ~Tguid() { }

  private:

    /// Cannot be constructed
    Tguid() { }

    /// static singleton instance
    static std::shared_ptr<Tguid> tguid_;

    /// Create an UUID random
    boost::uuids::random_generator idGenerator_;

    /// Allow shared_ptr with private constructors
    struct MakeSharedEnabler;

  };

  struct Tguid::MakeSharedEnabler : public Tguid {
    MakeSharedEnabler() : Tguid() { }
  };


  inline std::shared_ptr<Tguid> Tguid::GetInstance()
  {
    if (nullptr == tguid_)
    {
      tguid_ = std::make_shared<MakeSharedEnabler>();
    }
    return tguid_;
  };
  
};
