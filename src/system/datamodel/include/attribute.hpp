#pragma once

#include <baseObject.hpp>

#include <memory>

namespace gurps_system {

// Private types — consumers need the corresponding private headers to use
// the values returned by the derivation accessors.
class DerivationFormula;
class Formula;
class ParentRef;

/**
 * \brief A GURPS attribute definition — primary or secondary.
 *
 * Stores the handbook description of an attribute: its name, description, and
 * the formulas / derivation algorithm that define how its level is obtained.
 * Holds no per-character state; all computation is the responsibility of the
 * instance layer.
 *
 * Child layout (all optional, distinguished at runtime by \c dynamic_cast):
 *  1. \c DerivationFormula — algorithm descriptor for derived attributes.
 *  2. N \c ParentRef       — one per input attribute that feeds the formula.
 *  3. \c Formula           — CP-to-bonus curve for directly-bought levels.
 *                            \c Formula::maxDirectBonus caps the purchase (-1 = uncapped).
 *
 * Primary attributes (ST, DX, IQ, HT) have only the direct \c Formula.
 * Secondary attributes (HP, Will, ...) additionally carry a \c DerivationFormula and
 * one or more \c ParentRef children.
 *
 * \note Objects must be heap-allocated via \c std::make_shared.
 */
class Attribute : public gurps_system::BaseObject
{
  Q_OBJECT

public:
  explicit Attribute(boost::uuids::uuid objectId = boost::uuids::uuid{});
  ~Attribute() override;

  Attribute(Attribute const&) = delete;
  auto operator=(Attribute const&) -> Attribute& = delete;
  Attribute(Attribute&&) = delete;
  auto operator=(Attribute&&) -> Attribute& = delete;

  // ── Identity ──────────────────────────────────────────────────────────────

  static auto classId() -> boost::uuids::uuid;
  auto typeId() const -> boost::uuids::uuid override;

  // ── Direct formula (CP → bonus) ───────────────────────────────────────────

  /**
   * \brief Appends a CP-to-bonus formula as the last child.
   * \throws std::logic_error if a direct formula is already present.
   */
  auto insertDirectFormula(std::shared_ptr<Formula> formula) -> void;

  /** Returns \c true if a direct formula has been attached. */
  auto hasDirectFormula() const -> bool;

  /**
   * \brief Returns the direct formula.
   * \throws std::logic_error if no direct formula has been attached.
   */
  auto directFormula() const -> Formula const&;

  // ── Derivation (secondary attributes) ────────────────────────────────────

  /**
   * \brief Inserts the derivation formula as child 0.
   * \throws std::logic_error if a derivation formula is already present.
   */
  auto insertDerivationFormula(std::shared_ptr<DerivationFormula> formula) -> void;

  /** Returns the derivation formula, or \c nullptr if absent. */
  auto derivationFormula() const -> DerivationFormula*;

  /** Appends a parent reference, inserted before any direct-formula child. */
  auto addParent(std::shared_ptr<ParentRef> ref) -> void;

  /** Returns the number of parent references. */
  auto parentCount() const -> int;

  /**
   * \brief Returns the i-th parent reference.
   * \throws std::out_of_range if \p i is out of bounds.
   */
  auto parentAt(int i) const -> ParentRef const&;
};

} // namespace gurps_system
