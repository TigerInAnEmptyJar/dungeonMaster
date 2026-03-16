#include "simplePrerequisites.hpp"

#include "parentRef.hpp"

#include <boost/uuid/string_generator.hpp>

namespace gurps_system {

// ── AdvantagePrerequisite ─────────────────────────────────────────────────────

AdvantagePrerequisite::AdvantagePrerequisite(boost::uuids::uuid objectId)
    : SingleDependencyHolder{objectId}
{
}

AdvantagePrerequisite::~AdvantagePrerequisite() = default;

auto AdvantagePrerequisite::classId() -> boost::uuids::uuid
{
  static auto const id = boost::uuids::string_generator{}("e1f2a3b4-c5d6-4e7f-8a9b-0c1d2e3f4a5b");
  return id;
}

auto AdvantagePrerequisite::typeId() const -> boost::uuids::uuid { return classId(); }

auto AdvantagePrerequisite::advantageRef() const -> infrastructure::TreeItem*
{
  return dependency();
}

auto AdvantagePrerequisite::setAdvantageRef(infrastructure::TreeItem* ref) -> void
{
  setDependency(ref);
}

void AdvantagePrerequisite::emitDependencyChanged() { Q_EMIT advantageRefChanged(); }

// ── ProfessionPrerequisite ────────────────────────────────────────────────────

ProfessionPrerequisite::ProfessionPrerequisite(boost::uuids::uuid objectId)
    : SingleDependencyHolder{objectId}
{
}

ProfessionPrerequisite::~ProfessionPrerequisite() = default;

auto ProfessionPrerequisite::classId() -> boost::uuids::uuid
{
  static auto const id = boost::uuids::string_generator{}("a2b3c4d5-e6f7-48a9-0b1c-2d3e4f5a6b7c");
  return id;
}

auto ProfessionPrerequisite::typeId() const -> boost::uuids::uuid { return classId(); }

auto ProfessionPrerequisite::professionRef() const -> infrastructure::TreeItem*
{
  return dependency();
}

auto ProfessionPrerequisite::setProfessionRef(infrastructure::TreeItem* ref) -> void
{
  setDependency(ref);
}

void ProfessionPrerequisite::emitDependencyChanged() { Q_EMIT professionRefChanged(); }

// ── RacePrerequisite ──────────────────────────────────────────────────────────

RacePrerequisite::RacePrerequisite(boost::uuids::uuid objectId) : SingleDependencyHolder{objectId}
{
}

RacePrerequisite::~RacePrerequisite() = default;

auto RacePrerequisite::classId() -> boost::uuids::uuid
{
  static auto const id = boost::uuids::string_generator{}("b2c3d4e5-f6a7-489b-0c1d-2e3f4a5b6c7d");
  return id;
}

auto RacePrerequisite::typeId() const -> boost::uuids::uuid { return classId(); }

auto RacePrerequisite::raceRef() const -> infrastructure::TreeItem* { return dependency(); }

auto RacePrerequisite::setRaceRef(infrastructure::TreeItem* ref) -> void { setDependency(ref); }

void RacePrerequisite::emitDependencyChanged() { Q_EMIT raceRefChanged(); }

} // namespace gurps_system
