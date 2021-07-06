#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>

namespace liten
{

  /// UUID Generator wrapper
  class Tuuid
  {
  public:
    
    /// Create a new UUID
    static boost::uuids::uuid GenerateUuid()
    {
      return idGenerator_();
    }
    
  private:
    /// Cannot be constructed
    Tuuid() { }
    
    /// Create an UUID random
    static boost::uuids::random_generator idGenerator_;
    
  };
  
};
    
