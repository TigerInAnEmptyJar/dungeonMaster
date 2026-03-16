#include "compositePrerequisites.hpp"

#include <boost/uuid/string_generator.hpp>

namespace gurps_system {

// ── AndPrerequisite ───────────────────────────────────────────────────────────

AndPrerequisite::AndPrerequisite(boost::uuids::uuid objectId) : Prerequisite{objectId} {}

AndPrerequisite::~AndPrerequisite() = default;

auto AndPrerequisite::classId() -> boost::uuids::uuid
{
  static auto const id = boost::uuids::string_generator{}("e1f2a3b4-c5d6-47e8-9f0a-1b2c3d4e5f6a");
  return id;
}

auto AndPrerequisite::typeId() const -> boost::uuids::uuid { return classId(); }

// ── OrPrerequisite ────────────────────────────────────────────────────────────

OrPrerequisite::OrPrerequisite(boost::uuids::uuid objectId) : Prerequisite{objectId} {}

OrPrerequisite::~OrPrerequisite() = default;

auto OrPrerequisite::classId() -> boost::uuids::uuid
{
  static auto const id = boost::uuids::string_generator{}("f1a2b3c4-d5e6-478f-9a0b-1c2d3e4f5a6b");
  return id;
}

auto OrPrerequisite::typeId() const -> boost::uuids::uuid { return classId(); }

} // namespace gurps_system
