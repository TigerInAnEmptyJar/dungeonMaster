#include <profession.hpp>

#include <boost/uuid/string_generator.hpp>

namespace gurps_system {

// ── Construction ──────────────────────────────────────────────────────────────

Profession::Profession(boost::uuids::uuid objectId) : BaseObject{objectId} {}

Profession::~Profession() = default;

// ── Identity ──────────────────────────────────────────────────────────────────

auto Profession::classId() -> boost::uuids::uuid
{
  static auto const id = boost::uuids::string_generator{}("a1b2c3d4-e5f6-47a8-9b0c-1d2e3f4a5b6c");
  return id;
}

auto Profession::typeId() const -> boost::uuids::uuid { return classId(); }

} // namespace gurps_system
