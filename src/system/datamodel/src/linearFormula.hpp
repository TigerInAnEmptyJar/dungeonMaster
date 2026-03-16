#pragma once

#include <formula.hpp>

namespace gurps_system {

/**
 * \brief GURPS 4th edition formula: constant character point cost per level.
 *
 * Each level above the base costs exactly \c costPerLevel character points.
 * Examples (GURPS 4e):
 *   - Strength (ST) and Health (HT): 10 CP/level
 *   - Dexterity (DX) and Intelligence (IQ): 20 CP/level
 *   - Size Modifier (SM): 0 CP/level (free, use costPerLevel=1 or a custom formula)
 *
 * \c levelBonus(cp) truncates toward zero, so partial-level investments yield no
 * benefit (e.g. 15 CP with costPerLevel=10 → level bonus 1).
 *
 * \pre costPerLevel > 0
 */
class LinearFormula final : public Formula
{
  Q_OBJECT

  Q_PROPERTY(
      int costPerLevel READ costPerLevel WRITE setCostPerLevel DESIGNABLE false SCRIPTABLE false)

public:
  /**
   * \param costPerLevel  CP cost for each level raise. Must be > 0.
   * \param objectId      Optional stable identity; auto-generated when nil.
   */
  explicit LinearFormula(int costPerLevel, boost::uuids::uuid objectId = boost::uuids::uuid{});

  // ── Identity ──────────────────────────────────────────────────────────────

  static auto classId() -> boost::uuids::uuid;
  auto typeId() const -> boost::uuids::uuid override;

  // ── Formula ───────────────────────────────────────────────────────────────

  auto costPerLevel() const -> int;

  /**
   * \brief Sets the CP cost per level.
   *
   * Emits \c costPerLevelChanged if the value differs.
   * \throws std::invalid_argument if \p value <= 0.
   */
  auto setCostPerLevel(int value) -> void;

private:
  int _costPerLevel = 10;
};

} // namespace gurps_system
