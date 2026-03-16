#pragma once

#include <treeItem.hpp>

namespace gurps_system {

/**
 * \brief Abstract strategy for converting between invested character points and a level bonus.
 *
 * Extends \c infrastructure::TreeItem so that a concrete formula can be attached as a
 * child node of the owning object (Attribute, Skill, Advantage, …) and therefore
 * participates in the standard tree serialisation.
 *
 * Different GURPS editions use different costing schemes:
 *   - GURPS 4e: constant cost per level → LinearFormula
 *   - GURPS 3e: non-linear lookup table → LookupFormula
 *
 * Inject a concrete implementation into the owning object to select the ruleset.
 *
 * \note Both operations must satisfy the round-trip property for whole-level values:
 *       \c levelBonus(cpCost(n)) == n
 */
class Formula : public infrastructure::TreeItem
{
  Q_OBJECT

  Q_PROPERTY(
      int maxDirectBonus READ maxDirectBonus WRITE setMaxDirectBonus NOTIFY maxDirectBonusChanged)

public:
  explicit Formula(boost::uuids::uuid objectId = boost::uuids::uuid{});
  ~Formula() override = default;

  Formula(Formula const&) = delete;
  auto operator=(Formula const&) -> Formula& = delete;
  Formula(Formula&&) = delete;
  auto operator=(Formula&&) -> Formula& = delete;

  // ── Identity ────────────────────────────────────────────────────────────

  static auto classId() -> boost::uuids::uuid;
  // typeId() remains pure virtual — implemented by concrete subclasses

  // ── Direct-bonus cap ────────────────────────────────────────────────────

  /**
   * \brief Maximum number of levels that may be bought via this formula (-1 = uncapped).
   *
   * The instance layer reads this value to clamp the purchased bonus before
   * applying the formula.  A value of -1 (default) means no cap.
   */
  auto maxDirectBonus() const -> int;

  /**
   * \brief Sets the direct-bonus cap.
   */
  auto setMaxDirectBonus(int value) -> void;

  // ── Interface ───────────────────────────────────────────────────────────

  /**
   * \brief Returns the level bonus achieved by investing \p investedCp character points.
   */
  virtual auto levelBonus(int investedCp) const -> int = 0;

  /**
   * \brief Returns the character point cost to achieve the given \p levelBonus.
   */
  virtual auto cpCost(int levelBonus) const -> int = 0;

Q_SIGNALS:
  void maxDirectBonusChanged(int value);

private:
  int _maxDirectBonus{-1};
};

} // namespace gurps_system
