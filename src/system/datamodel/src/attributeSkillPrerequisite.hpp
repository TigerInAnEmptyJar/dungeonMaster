#pragma once

#include "prerequisite.hpp"
#include "singleParentRefHolder.hpp"

namespace gurps_system {

// Forward declaration
class ParentRef;

/**
 * \brief Prerequisite requiring an attribute to be at or above a minimum value.
 *
 * Example: Learning "Spellcasting" requires IQ >= 12.
 *
 * Child layout:
 *  1. \c ParentRef — reference to the attribute
 *
 * \note Objects must be heap-allocated via \c std::make_shared.
 */
class AttributePrerequisite : public SingleDependencyHolder<AttributePrerequisite, Prerequisite>
{
  Q_OBJECT

  Q_PROPERTY(int minimumValue READ minimumValue WRITE setMinimumValue NOTIFY minimumValueChanged)

public:
  explicit AttributePrerequisite(boost::uuids::uuid objectId = boost::uuids::uuid{});
  ~AttributePrerequisite() override;

  AttributePrerequisite(AttributePrerequisite const&) = delete;
  auto operator=(AttributePrerequisite const&) -> AttributePrerequisite& = delete;
  AttributePrerequisite(AttributePrerequisite&&) = delete;
  auto operator=(AttributePrerequisite&&) -> AttributePrerequisite& = delete;

  // ── Identity ──────────────────────────────────────────────────────────────

  static auto classId() -> boost::uuids::uuid;
  auto typeId() const -> boost::uuids::uuid override;

  // ── Properties ────────────────────────────────────────────────────────────

  /**
   * \brief Returns the minimum attribute value required.
   */
  auto minimumValue() const -> int;

  /**
   * \brief Sets the minimum attribute value required.
   *
   * Emits \c minimumValueChanged if the value differs from the current one.
   */
  auto setMinimumValue(int value) -> void;

  // ── Attribute reference ───────────────────────────────────────────────────

  /**
   * \brief Returns the reference to the required attribute.
   *
   * The attribute reference is stored as the first child.
   * Returns \c nullptr if no attribute reference has been set.
   */
  Q_INVOKABLE infrastructure::TreeItem* attributeRef() const;

  /**
   * \brief Sets the attribute reference for this prerequisite.
   *
   * Replaces the first child if it already exists, otherwise inserts at position 0.
   * Emits \c attributeRefChanged.
   *
   * \param ref  The ParentRef to the attribute. Must be a ParentRef*.
   */
  Q_INVOKABLE void setAttributeRef(infrastructure::TreeItem* ref);

Q_SIGNALS:
  void minimumValueChanged(int value);
  void attributeRefChanged();

protected:
  void emitDependencyChanged() override;

private:
  struct Impl;
  std::unique_ptr<Impl> _p;
};

/**
 * \brief Prerequisite requiring a skill to be at or above a minimum value.
 *
 * Example: Learning "Surgery" requires "First Aid" >= 12.
 *
 * Child layout:
 *  1. \c ParentRef — reference to the required skill
 *
 * \note Objects must be heap-allocated via \c std::make_shared.
 */
class SkillPrerequisite : public SingleDependencyHolder<SkillPrerequisite, Prerequisite>
{
  Q_OBJECT

  Q_PROPERTY(int minimumValue READ minimumValue WRITE setMinimumValue NOTIFY minimumValueChanged)

public:
  explicit SkillPrerequisite(boost::uuids::uuid objectId = boost::uuids::uuid{});
  ~SkillPrerequisite() override;

  SkillPrerequisite(SkillPrerequisite const&) = delete;
  auto operator=(SkillPrerequisite const&) -> SkillPrerequisite& = delete;
  SkillPrerequisite(SkillPrerequisite&&) = delete;
  auto operator=(SkillPrerequisite&&) -> SkillPrerequisite& = delete;

  // ── Identity ──────────────────────────────────────────────────────────────

  static auto classId() -> boost::uuids::uuid;
  auto typeId() const -> boost::uuids::uuid override;

  // ── Properties ────────────────────────────────────────────────────────────

  /**
   * \brief Returns the minimum skill value required.
   */
  auto minimumValue() const -> int;

  /**
   * \brief Sets the minimum skill value required.
   *
   * Emits \c minimumValueChanged if the value differs from the current one.
   */
  auto setMinimumValue(int value) -> void;

  // ── Skill reference ───────────────────────────────────────────────────────

  /**
   * \brief Returns the reference to the required skill.
   *
   * The skill reference is stored as the first child.
   * Returns \c nullptr if no skill reference has been set.
   */
  Q_INVOKABLE infrastructure::TreeItem* skillRef() const;

  /**
   * \brief Sets the skill reference for this prerequisite.
   *
   * Replaces the first child if it already exists, otherwise inserts at position 0.
   * Emits \c skillRefChanged.
   *
   * \param ref  The ParentRef to the skill. Must be a ParentRef*.
   */
  Q_INVOKABLE void setSkillRef(infrastructure::TreeItem* ref);

Q_SIGNALS:
  void minimumValueChanged(int value);
  void skillRefChanged();

protected:
  void emitDependencyChanged() override;

private:
  struct Impl;
  std::unique_ptr<Impl> _p;
};

} // namespace gurps_system
