#include "skillDefault.hpp"

#include "parentRef.hpp"

#include <boost/uuid/string_generator.hpp>

namespace gurps_system {

// ── Pimpl ─────────────────────────────────────────────────────────────────────

struct SkillDefault::Impl
{
  int _modifier{0};
};

// ── Lifecycle ─────────────────────────────────────────────────────────────────

SkillDefault::SkillDefault(boost::uuids::uuid objectId)
    : SingleDependencyHolder{objectId}, _p(std::make_unique<Impl>())
{
}

SkillDefault::~SkillDefault() = default;

// ── Identity ──────────────────────────────────────────────────────────────────

auto SkillDefault::classId() -> boost::uuids::uuid
{
  static auto const id = boost::uuids::string_generator{}("f1a2b3c4-d5e6-4f7a-8b9c-0d1e2f3a4b5c");
  return id;
}

auto SkillDefault::typeId() const -> boost::uuids::uuid { return classId(); }

// ── Properties ────────────────────────────────────────────────────────────────

auto SkillDefault::modifier() const -> int { return _p->_modifier; }

auto SkillDefault::setModifier(int modifier) -> void
{
  if (_p->_modifier == modifier) {
    return;
  }
  _p->_modifier = modifier;
  Q_EMIT modifierChanged(_p->_modifier);
}

// ── Target reference ──────────────────────────────────────────────────────────

auto SkillDefault::target() const -> infrastructure::TreeItem* { return dependency(); }

auto SkillDefault::setTarget(infrastructure::TreeItem* ref) -> void { setDependency(ref); }

void SkillDefault::emitDependencyChanged() { Q_EMIT targetChanged(); }

} // namespace gurps_system
