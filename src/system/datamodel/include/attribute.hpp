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

  // ── Formula access ────────────────────────────────────────────────────────

  /**
   * \brief Returns the formula as a QObject pointer for unified access.
   *
   * Returns the formula at position 0 (either DerivationFormula or Formula).
   * This unified accessor allows QML and C++ code to work with both formula types through
   * the same interface.
   *
   * \returns The formula object, or nullptr if no formula has been attached.
   */
  Q_INVOKABLE infrastructure::TreeItem* formula() const;

  /**
   * \brief Sets (replaces) the formula by swapping child 0.
   *
   * If a formula already exists at position 0, it is replaced.
   * Otherwise, the formula is inserted at position 0.
   * Emits formulaChanged() signal.
   *
   * \param formula The new formula to set (must inherit from Formula)
   */
  Q_INVOKABLE void setFormula(infrastructure::TreeItem* aFormula);

Q_SIGNALS:
  void formulaChanged();
};

} // namespace gurps_system
