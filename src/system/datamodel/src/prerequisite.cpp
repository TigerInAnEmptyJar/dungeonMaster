#include "prerequisite.hpp"

#include <boost/uuid/string_generator.hpp>

namespace gurps_system {

// ── Lifecycle ─────────────────────────────────────────────────────────────────

Prerequisite::Prerequisite(boost::uuids::uuid objectId) : infrastructure::TreeItem{objectId} {}

Prerequisite::~Prerequisite() = default;

// ── Identity ──────────────────────────────────────────────────────────────────

auto Prerequisite::classId() -> boost::uuids::uuid
{
  static auto const id = boost::uuids::string_generator{}("b1c2d3e4-f5a6-4b7c-8d9e-0f1a2b3c4d5e");
  return id;
}

auto Prerequisite::typeId() const -> boost::uuids::uuid { return classId(); }

} // namespace gurps_system
