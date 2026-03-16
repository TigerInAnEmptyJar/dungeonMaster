#include <race.hpp>

#include <boost/uuid/string_generator.hpp>

namespace gurps_system {

// ── Construction ──────────────────────────────────────────────────────────────

Race::Race(boost::uuids::uuid objectId) : BaseObject{objectId} {}

Race::~Race() = default;

// ── Identity ──────────────────────────────────────────────────────────────────

auto Race::classId() -> boost::uuids::uuid
{
  static auto const id = boost::uuids::string_generator{}("f1e2d3c4-b5a6-4798-8b9c-0a1b2c3d4e5f");
  return id;
}

auto Race::typeId() const -> boost::uuids::uuid { return classId(); }

} // namespace gurps_system
