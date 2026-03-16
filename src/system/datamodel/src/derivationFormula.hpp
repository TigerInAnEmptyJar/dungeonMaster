#pragma once

#include <formula.hpp>

namespace gurps_system {

/**
 * \brief Abstract descriptor for a secondary-attribute derivation algorithm.
 *
 * A \c DerivationFormula is a pure parameter bag: it records the algorithm
 * type and its configuration values, but performs no computation itself.
 * Instance-level code dispatches on \c typeId() to select the appropriate
 * evaluation strategy and reads the parameters from the concrete subclass.
 *
 * Inherits from Formula to provide a unified base class for all formula types.
 * The CP-related methods (levelBonus/cpCost) are not applicable and will throw.
 *
 * Concrete subclasses:
 *   - \c ScaledSumDerivationFormula  – weighted sum divided by a divisor
 *   - \c QuadraticDerivationFormula  – single input squared, divided by a divisor
 *   - \c LookupDerivationFormula     – direct table lookup
 *
 * Stored as a child of \c SecondaryAttribute, identified at runtime by
 * \c dynamic_cast<DerivationFormula*>.
 */
class DerivationFormula : public Formula
{
  Q_OBJECT

public:
  explicit DerivationFormula(boost::uuids::uuid objectId = boost::uuids::uuid{});
  ~DerivationFormula() override = default;

  DerivationFormula(DerivationFormula const&) = delete;
  auto operator=(DerivationFormula const&) -> DerivationFormula& = delete;
  DerivationFormula(DerivationFormula&&) = delete;
  auto operator=(DerivationFormula&&) -> DerivationFormula& = delete;

  // ── Identity ──────────────────────────────────────────────────────────────

  static auto classId() -> boost::uuids::uuid;
  // typeId() remains pure virtual through Formula;
  // concrete subclasses must override it.
};

} // namespace gurps_system
