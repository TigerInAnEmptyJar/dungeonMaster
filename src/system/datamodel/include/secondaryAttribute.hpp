#pragma once

#include <baseObject.hpp>

#include <memory>

namespace gurps_system {

// Private types — consumers need to include the corresponding private headers
// to use the returned references/pointers.
class DerivationFormula;
class Formula;
class ParentRef;

/**
 * \brief System-layer descriptor of a GURPS secondary attribute.
 *
 * A \c SecondaryAttribute is a pure description: it records \em how an
 * attribute is derived, but never calculates the actual level.  Computation
 * is the responsibility of the instance layer.
 *
 * Child layout (all optional, identified at runtime by \c dynamic_cast):
 *  1. \c DerivationFormula — at child index 0; describes the derivation
 *     algorithm and its parameters.
 *  2. N × \c ParentRef — immediately after the formula (or at 0 if there is
 *     no formula); each points to a primary attribute that feeds the formula.
 *  3. Direct \c Formula — appended last; describes the CP-to-bonus curve for
 *     buying direct bonuses (like bought-up HP).
 *
 * Example: \c BasicSpeed = (DX + HT) / 4
 *  - DerivationFormula: \c ScaledSumDerivationFormula{ coefficients=[1,1], divisor=4 }
 *  - ParentRef → DX
 *  - ParentRef → HT
 *
 * The \c maxDirectBonus property caps how many levels may be bought directly.
 * A value of -1 (default) means no cap.
 *
 * \note SecondaryAttribute objects must be heap-allocated via \c std::make_shared.
 */
class SecondaryAttribute : public gurps_system::BaseObject
{
  Q_OBJECT

  Q_PROPERTY(
      int maxDirectBonus READ maxDirectBonus WRITE setMaxDirectBonus NOTIFY maxDirectBonusChanged)

public:
  /**
   * \param objectId  Optional stable identity.
   */
  explicit SecondaryAttribute(boost::uuids::uuid objectId = boost::uuids::uuid{});
  ~SecondaryAttribute() override;

  SecondaryAttribute(SecondaryAttribute const&) = delete;
  auto operator=(SecondaryAttribute const&) -> SecondaryAttribute& = delete;
  SecondaryAttribute(SecondaryAttribute&&) = delete;
  auto operator=(SecondaryAttribute&&) -> SecondaryAttribute& = delete;

  // ── Identity ──────────────────────────────────────────────────────────────

  static auto classId() -> boost::uuids::uuid;
  auto typeId() const -> boost::uuids::uuid override;

  // ── Properties ────────────────────────────────────────────────────────────

  /**
   * \brief Maximum number of levels that may be bought directly (via the direct formula).
   *
   * -1 means unlimited.  Ignored when there is no direct formula.
   */
  auto maxDirectBonus() const -> int;

  /**
   * \brief Sets the direct-bonus cap.
   */
  auto setMaxDirectBonus(int value) -> void;

  // ── Derivation setup ──────────────────────────────────────────────────────

  /**
   * \brief Inserts the derivation formula as child 0.
   *
   * \throws std::logic_error if a derivation formula is already present.
   */
  auto insertDerivationFormula(std::shared_ptr<DerivationFormula> formula) -> void;

  /**
   * \brief Appends a parent reference, before any direct-formula child.
   */
  auto addParent(std::shared_ptr<ParentRef> ref) -> void;

  /**
   * \brief Appends a direct CP-to-bonus formula as the last child.
   *
   * \throws std::logic_error if a direct formula is already present.
   */
  auto insertDirectFormula(std::shared_ptr<Formula> formula) -> void;

  // ── Accessors ─────────────────────────────────────────────────────────────

  /**
   * \brief Returns the derivation formula, or \c nullptr if absent.
   */
  auto derivationFormula() const -> DerivationFormula*;

  /**
   * \brief Returns the number of parent references.
   */
  auto parentCount() const -> int;

  /**
   * \brief Returns the i-th parent reference.
   * \throws std::out_of_range if \p i is out of bounds.
   */
  auto parentAt(int i) const -> ParentRef const&;

  /**
   * \brief Returns \c true if a direct formula has been attached.
   */
  auto hasDirectFormula() const -> bool;

  /**
   * \brief Returns the direct formula.
   * \throws std::logic_error if no direct formula has been attached.
   */
  auto directFormula() const -> Formula const&;

Q_SIGNALS:
  void maxDirectBonusChanged(int value);

private:
  int _maxDirectBonus{-1};
};

} // namespace gurps_system
