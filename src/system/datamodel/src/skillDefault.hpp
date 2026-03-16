#pragma once

#include "singleParentRefHolder.hpp"

namespace gurps_system {

// Forward declaration
class ParentRef;

/**
 * \brief Represents a default for using a skill untrained.
 *
 * A default allows a skill to be used at a penalty when the character
 * has not directly learned the skill. The default consists of:
 *  - A reference to a skill or attribute (stored as a ParentRef at position 0)
 *  - An integer modifier (typically negative)
 *
 * Example: "First Aid" skill might default to IQ-4 or "Physician" skill-5.
 *
 * Child layout:
 *  1. \c ParentRef — reference to the default source (skill or attribute)
 *
 * Multiple SkillDefault objects can be children of a Skill, allowing
 * a skill to have several different defaults.
 *
 * \note Objects must be heap-allocated via \c std::make_shared.
 */
class SkillDefault : public SingleDependencyHolder<SkillDefault, infrastructure::TreeItem>
{
  Q_OBJECT

  Q_PROPERTY(int modifier READ modifier WRITE setModifier NOTIFY modifierChanged)

public:
  explicit SkillDefault(boost::uuids::uuid objectId = boost::uuids::uuid{});
  ~SkillDefault() override;

  SkillDefault(SkillDefault const&) = delete;
  auto operator=(SkillDefault const&) -> SkillDefault& = delete;
  SkillDefault(SkillDefault&&) = delete;
  auto operator=(SkillDefault&&) -> SkillDefault& = delete;

  // ── Identity ──────────────────────────────────────────────────────────────

  static auto classId() -> boost::uuids::uuid;
  auto typeId() const -> boost::uuids::uuid override;

  // ── Properties ────────────────────────────────────────────────────────────

  /**
   * \brief Returns the modifier applied to the default source.
   *
   * Typically negative (e.g., -4 for "IQ-4").
   */
  auto modifier() const -> int;

  /**
   * \brief Sets the modifier applied to the default source.
   *
   * Emits \c modifierChanged if the value differs from the current one.
   */
  auto setModifier(int modifier) -> void;

  // ── Target reference ──────────────────────────────────────────────────────

  /**
   * \brief Returns the reference to the default source (skill or attribute).
   *
   * The target reference is stored as the first child of the SkillDefault.
   * Returns \c nullptr if no target reference has been set.
   */
  Q_INVOKABLE infrastructure::TreeItem* target() const;

  /**
   * \brief Sets the target reference for this default.
   *
   * Replaces the first child if it already exists, otherwise inserts at position 0.
   * Emits \c targetChanged.
   *
   * \param ref  The ParentRef to the skill or attribute. Must be a ParentRef*.
   */
  Q_INVOKABLE void setTarget(infrastructure::TreeItem* ref);

Q_SIGNALS:
  void modifierChanged(int modifier);
  void targetChanged();

protected:
  void emitDependencyChanged() override;

private:
  struct Impl;
  std::unique_ptr<Impl> _p;
};

} // namespace gurps_system
