#include "raceContainer.hpp"

#include <boost/uuid/string_generator.hpp>

namespace gurps_system {

RaceContainer::RaceContainer() : infrastructure::TreeItem() {}

boost::uuids::uuid const RaceContainer::raceContainerId =
    boost::uuids::string_generator{}("b1a2c3d4-e5f6-47g8-9h0i-1j2k3l4m5n6o");

auto RaceContainer::classId() -> boost::uuids::uuid
{
  static auto const id = boost::uuids::string_generator{}("c2b3d4e5-f6a7-48h9-0i1j-2k3l4m5n6o7p");
  return id;
}

auto RaceContainer::typeId() const -> boost::uuids::uuid { return classId(); }

} // namespace gurps_system
