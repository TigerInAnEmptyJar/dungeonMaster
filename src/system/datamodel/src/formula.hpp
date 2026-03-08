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

  // ── Interface ───────────────────────────────────────────────────────────

  /**
   * \brief Returns the level bonus achieved by investing \p investedCp character points.
   *
   * A positive result means the item is raised above its base level.
   */
  virtual auto levelBonus(int investedCp) const -> int = 0;

  /**
   * \brief Returns the character point cost to achieve the given \p levelBonus.
   *
   * The inverse of \c levelBonus: \c cpCost(levelBonus(cp)) == cp for multiples
   * of the cost quantum.
   */
  virtual auto cpCost(int levelBonus) const -> int = 0;
};

} // namespace gurps_system
