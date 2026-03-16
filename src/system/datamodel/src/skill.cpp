#include <skill.hpp>

#include "parentRef.hpp"

#include <boost/uuid/string_generator.hpp>

namespace gurps_system {

// ── Pimpl ─────────────────────────────────────────────────────────────────────

struct Skill::Impl
{
  int _difficulty{0};
  SkillType _type{SkillType::Mental};
};

// ── Lifecycle ─────────────────────────────────────────────────────────────────

Skill::Skill(boost::uuids::uuid objectId) : BaseObject{objectId}, _p(std::make_unique<Impl>())
{
  connect(this, &Skill::childInserted, this, [this](int index) {
    // Emit attributeRefChanged if the inserted child at position 0 is a ParentRef
    if (index == 0 && dynamic_cast<ParentRef*>(childAt(0).get())) {
      Q_EMIT attributeRefChanged();
    }
  });

  connect(this, &Skill::childAboutToBeRemoved, this, [this](int index) {
    // Emit attributeRefChanged if the removed child at position 0 is a ParentRef
    if (index == 0 && dynamic_cast<ParentRef*>(childAt(0).get())) {
      Q_EMIT attributeRefChanged();
    }
  });
}

Skill::~Skill() = default;

// ── Identity ──────────────────────────────────────────────────────────────────

auto Skill::classId() -> boost::uuids::uuid
{
  static auto const id = boost::uuids::string_generator{}("a1b2c3d4-e5f6-4a7b-8c9d-0e1f2a3b4c5d");
  return id;
}

auto Skill::typeId() const -> boost::uuids::uuid { return classId(); }

// ── Properties ────────────────────────────────────────────────────────────────

auto Skill::difficulty() const -> int { return _p->_difficulty; }

auto Skill::setDifficulty(int difficulty) -> void
{
  if (_p->_difficulty == difficulty) {
    return;
  }
  _p->_difficulty = difficulty;
  Q_EMIT difficultyChanged(_p->_difficulty);
}

auto Skill::type() const -> SkillType { return _p->_type; }

auto Skill::setType(SkillType type) -> void
{
  if (_p->_type == type) {
    return;
  }
  _p->_type = type;
  Q_EMIT typeChanged(_p->_type);
}

// ── Attribute reference ───────────────────────────────────────────────────────

auto Skill::attributeRef() const -> infrastructure::TreeItem*
{
  // Check first child for ParentRef
  if (size() > 0) {
    if (auto ref = std::dynamic_pointer_cast<ParentRef>(childAt(0))) {
      return ref.get();
    }
  }
  return nullptr;
}

auto Skill::setAttributeRef(infrastructure::TreeItem* ref) -> void
{
  if (ref && !dynamic_cast<ParentRef*>(ref)) {
    return; // Not a ParentRef, do nothing
  }

  if (size() > 0) {
    // Replace or remove existing ParentRef at position 0
    if (ref) {
      this->swapChild(childAt(0), ref->shared_from_this());
    } else {
      // Remove the ParentRef
      this->removeChild(childAt(0));
    }
  } else if (ref) {
    // Insert new ParentRef at position 0
    insertChild(0, ref->shared_from_this());
  }
}

} // namespace gurps_system
