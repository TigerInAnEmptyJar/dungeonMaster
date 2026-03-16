#pragma once

#include <baseObject.hpp>

#include <memory>

namespace gurps_system {

// Forward declaration for attribute reference
class ParentRef;

/**
 * \brief A GURPS skill definition.
 *
 * Stores the skill's difficulty level and type (mental or physical).
 * The skill references an attribute through a ParentRef stored as its first child.
 *
 * Child layout:
 *  1. \c ParentRef — reference to the controlling attribute for this skill.
 *
 * \note Objects must be heap-allocated via \c std::make_shared.
 */
class Skill : public BaseObject
{
  Q_OBJECT

  Q_PROPERTY(int difficulty READ difficulty WRITE setDifficulty NOTIFY difficultyChanged)
  Q_PROPERTY(SkillType type READ type WRITE setType NOTIFY typeChanged)

public:
  /**
   * \brief Enumeration of skill types.
   */
  enum class SkillType
  {
    Mental = 0,  ///< Mental skill (IQ-based typically)
    Physical = 1 ///< Physical skill (DX-based typically)
  };
  Q_ENUM(SkillType)

  explicit Skill(boost::uuids::uuid objectId = boost::uuids::uuid{});
  ~Skill() override;

  Skill(Skill const&) = delete;
  auto operator=(Skill const&) -> Skill& = delete;
  Skill(Skill&&) = delete;
  auto operator=(Skill&&) -> Skill& = delete;

  // ── Identity ──────────────────────────────────────────────────────────────

  static auto classId() -> boost::uuids::uuid;
  auto typeId() const -> boost::uuids::uuid override;

  // ── Properties ────────────────────────────────────────────────────────────

  /**
   * \brief Returns the difficulty level of this skill.
   */
  auto difficulty() const -> int;

  /**
   * \brief Sets the difficulty level of this skill.
   *
   * Emits \c difficultyChanged if the value differs from the current one.
   */
  auto setDifficulty(int difficulty) -> void;

  /**
   * \brief Returns the type of this skill (mental or physical).
   */
  auto type() const -> SkillType;

  /**
   * \brief Sets the type of this skill.
   *
   * Emits \c typeChanged if the value differs from the current one.
   */
  auto setType(SkillType type) -> void;

  // ── Attribute reference ───────────────────────────────────────────────────

  /**
   * \brief Returns the attribute reference (ParentRef) for this skill.
   *
   * The attribute reference is stored as the first child of the skill.
   * Returns \c nullptr if no attribute reference has been set.
   */
  Q_INVOKABLE infrastructure::TreeItem* attributeRef() const;

  /**
   * \brief Sets the attribute reference for this skill.
   *
   * Replaces the first child if it already exists, otherwise inserts at position 0.
   * Emits \c attributeRefChanged.
   *
   * \param ref  The ParentRef to the controlling attribute. Must be a ParentRef*.
   */
  Q_INVOKABLE void setAttributeRef(infrastructure::TreeItem* ref);

Q_SIGNALS:
  void difficultyChanged(int difficulty);
  void typeChanged(SkillType type);
  void attributeRefChanged();

private:
  struct Impl;
  std::unique_ptr<Impl> _p;
};

} // namespace gurps_system
