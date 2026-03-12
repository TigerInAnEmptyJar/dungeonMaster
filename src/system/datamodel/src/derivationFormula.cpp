#include <derivationFormula.hpp>

#include <boost/uuid/string_generator.hpp>

namespace gurps_system {

DerivationFormula::DerivationFormula(boost::uuids::uuid objectId)
    : infrastructure::TreeItem{objectId}
{
}

auto DerivationFormula::classId() -> boost::uuids::uuid
{
  static auto const id = boost::uuids::string_generator{}("f5a6b7c8-d9e0-4f1a-2b3c-4d5e6f7a8b9c");
  return id;
}

} // namespace gurps_system
