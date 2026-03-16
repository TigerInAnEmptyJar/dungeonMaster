#pragma once

#include "prerequisite.hpp"
#include "singleParentRefHolder.hpp"

namespace gurps_system {

// Forward declaration
class ParentRef;

/**
 * \brief Prerequisite requiring possession of a specific advantage.
 *
 * Example: Learning "Magic Missile" requires the "Magery" advantage.
 *
 * Child layout:
 *  1. \c ParentRef — reference to the required advantage
 *
 * \note Objects must be heap-allocated via \c std::make_shared.
 * \note Advantage class does not exist yet; this is prepared for future use.
 */
class AdvantagePrerequisite : public SingleDependencyHolder<AdvantagePrerequisite, Prerequisite>
{
  Q_OBJECT

public:
  explicit AdvantagePrerequisite(boost::uuids::uuid objectId = boost::uuids::uuid{});
  ~AdvantagePrerequisite() override;

  AdvantagePrerequisite(AdvantagePrerequisite const&) = delete;
  auto operator=(AdvantagePrerequisite const&) -> AdvantagePrerequisite& = delete;
  AdvantagePrerequisite(AdvantagePrerequisite&&) = delete;
  auto operator=(AdvantagePrerequisite&&) -> AdvantagePrerequisite& = delete;

  // ── Identity ──────────────────────────────────────────────────────────────

  static auto classId() -> boost::uuids::uuid;
  auto typeId() const -> boost::uuids::uuid override;

  // ── Advantage reference ───────────────────────────────────────────────────

  /**
   * \brief Returns the reference to the required advantage.
   *
   * The advantage reference is stored as the first child.
   * Returns \c nullptr if no advantage reference has been set.
   */
  Q_INVOKABLE infrastructure::TreeItem* advantageRef() const;

  /**
   * \brief Sets the advantage reference for this prerequisite.
   *
   * Replaces the first child if it already exists, otherwise inserts at position 0.
   * Emits \c advantageRefChanged.
   *
   * \param ref  The ParentRef to the advantage. Must be a ParentRef*.
   */
  Q_INVOKABLE void setAdvantageRef(infrastructure::TreeItem* ref);

Q_SIGNALS:
  void advantageRefChanged();

protected:
  void emitDependencyChanged() override;
};

/**
 * \brief Prerequisite requiring a specific profession.
 *
 * Example: Some high-level medical skills require the "Doctor" profession.
 *
 * Child layout:
 *  1. \c ParentRef — reference to the required profession
 *
 * \note Objects must be heap-allocated via \c std::make_shared.
 */
class ProfessionPrerequisite : public SingleDependencyHolder<ProfessionPrerequisite, Prerequisite>
{
  Q_OBJECT

public:
  explicit ProfessionPrerequisite(boost::uuids::uuid objectId = boost::uuids::uuid{});
  ~ProfessionPrerequisite() override;

  ProfessionPrerequisite(ProfessionPrerequisite const&) = delete;
  auto operator=(ProfessionPrerequisite const&) -> ProfessionPrerequisite& = delete;
  ProfessionPrerequisite(ProfessionPrerequisite&&) = delete;
  auto operator=(ProfessionPrerequisite&&) -> ProfessionPrerequisite& = delete;

  // ── Identity ──────────────────────────────────────────────────────────────

  static auto classId() -> boost::uuids::uuid;
  auto typeId() const -> boost::uuids::uuid override;

  // ── Profession reference ──────────────────────────────────────────────────

  /**
   * \brief Returns the reference to the required profession.
   *
   * The profession reference is stored as the first child.
   * Returns \c nullptr if no profession reference has been set.
   */
  Q_INVOKABLE infrastructure::TreeItem* professionRef() const;

  /**
   * \brief Sets the profession reference for this prerequisite.
   *
   * Replaces the first child if it already exists, otherwise inserts at position 0.
   * Emits \c professionRefChanged.
   *
   * \param ref  The ParentRef to the profession. Must be a ParentRef*.
   */
  Q_INVOKABLE void setProfessionRef(infrastructure::TreeItem* ref);

Q_SIGNALS:
  void professionRefChanged();

protected:
  void emitDependencyChanged() override;
};

/**
 * \brief Prerequisite requiring a specific race.
 *
 * Example: Some racial skills require the character to be a specific race.
 *
 * Child layout:
 *  1. \c ParentRef — reference to the required race
 *
 * \note Objects must be heap-allocated via \c std::make_shared.
 */
class RacePrerequisite : public SingleDependencyHolder<RacePrerequisite, Prerequisite>
{
  Q_OBJECT

public:
  explicit RacePrerequisite(boost::uuids::uuid objectId = boost::uuids::uuid{});
  ~RacePrerequisite() override;

  RacePrerequisite(RacePrerequisite const&) = delete;
  auto operator=(RacePrerequisite const&) -> RacePrerequisite& = delete;
  RacePrerequisite(RacePrerequisite&&) = delete;
  auto operator=(RacePrerequisite&&) -> RacePrerequisite& = delete;

  // ── Identity ──────────────────────────────────────────────────────────────

  static auto classId() -> boost::uuids::uuid;
  auto typeId() const -> boost::uuids::uuid override;

  // ── Race reference ────────────────────────────────────────────────────────

  /**
   * \brief Returns the reference to the required race.
   *
   * The race reference is stored as the first child.
   * Returns \c nullptr if no race reference has been set.
   */
  Q_INVOKABLE infrastructure::TreeItem* raceRef() const;

  /**
   * \brief Sets the race reference for this prerequisite.
   *
   * Replaces the first child if it already exists, otherwise inserts at position 0.
   * Emits \c raceRefChanged.
   *
   * \param ref  The ParentRef to the race. Must be a ParentRef*.
   */
  Q_INVOKABLE void setRaceRef(infrastructure::TreeItem* ref);

Q_SIGNALS:
  void raceRefChanged();

protected:
  void emitDependencyChanged() override;
};

} // namespace gurps_system
