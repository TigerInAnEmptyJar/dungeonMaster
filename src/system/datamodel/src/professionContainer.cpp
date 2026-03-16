#include "professionContainer.hpp"

#include <boost/uuid/string_generator.hpp>

namespace gurps_system {

ProfessionContainer::ProfessionContainer() : infrastructure::TreeItem() {}

boost::uuids::uuid const ProfessionContainer::professionContainerId =
    boost::uuids::string_generator{}("d4c3b2a1-0f9e-48d7-bc6a-5f4e3d2c1b0a");

auto ProfessionContainer::classId() -> boost::uuids::uuid
{
  static auto const id = boost::uuids::string_generator{}("e5d4c3b2-1a0f-49e8-cd7b-6a5f4e3d2c1b");
  return id;
}

auto ProfessionContainer::typeId() const -> boost::uuids::uuid { return classId(); }

} // namespace gurps_system
