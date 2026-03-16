#include "attributeSkillPrerequisite.hpp"

#include "parentRef.hpp"

#include <boost/uuid/string_generator.hpp>

namespace gurps_system {

// ── AttributePrerequisite ─────────────────────────────────────────────────────

struct AttributePrerequisite::Impl
{
  int _minimumValue{0};
};

AttributePrerequisite::AttributePrerequisite(boost::uuids::uuid objectId)
    : SingleDependencyHolder{objectId}, _p(std::make_unique<Impl>())
{
}

AttributePrerequisite::~AttributePrerequisite() = default;

auto AttributePrerequisite::classId() -> boost::uuids::uuid
{
  static auto const id = boost::uuids::string_generator{}("c1d2e3f4-a5b6-4c7d-8e9f-0a1b2c3d4e5f");
  return id;
}

auto AttributePrerequisite::typeId() const -> boost::uuids::uuid { return classId(); }

auto AttributePrerequisite::minimumValue() const -> int { return _p->_minimumValue; }

auto AttributePrerequisite::setMinimumValue(int value) -> void
{
  if (_p->_minimumValue == value) {
    return;
  }
  _p->_minimumValue = value;
  Q_EMIT minimumValueChanged(_p->_minimumValue);
}

auto AttributePrerequisite::attributeRef() const -> infrastructure::TreeItem*
{
  return dependency();
}

auto AttributePrerequisite::setAttributeRef(infrastructure::TreeItem* ref) -> void
{
  setDependency(ref);
}

void AttributePrerequisite::emitDependencyChanged() { Q_EMIT attributeRefChanged(); }

// ── SkillPrerequisite ─────────────────────────────────────────────────────────

struct SkillPrerequisite::Impl
{
  int _minimumValue{0};
};

SkillPrerequisite::SkillPrerequisite(boost::uuids::uuid objectId)
    : SingleDependencyHolder{objectId}, _p(std::make_unique<Impl>())
{
}

SkillPrerequisite::~SkillPrerequisite() = default;

auto SkillPrerequisite::classId() -> boost::uuids::uuid
{
  static auto const id = boost::uuids::string_generator{}("d1e2f3a4-b5c6-4d7e-8f9a-0b1c2d3e4f5a");
  return id;
}

auto SkillPrerequisite::typeId() const -> boost::uuids::uuid { return classId(); }

auto SkillPrerequisite::minimumValue() const -> int { return _p->_minimumValue; }

auto SkillPrerequisite::setMinimumValue(int value) -> void
{
  if (_p->_minimumValue == value) {
    return;
  }
  _p->_minimumValue = value;
  Q_EMIT minimumValueChanged(_p->_minimumValue);
}

auto SkillPrerequisite::skillRef() const -> infrastructure::TreeItem* { return dependency(); }

auto SkillPrerequisite::setSkillRef(infrastructure::TreeItem* ref) -> void { setDependency(ref); }

void SkillPrerequisite::emitDependencyChanged() { Q_EMIT skillRefChanged(); }

} // namespace gurps_system
