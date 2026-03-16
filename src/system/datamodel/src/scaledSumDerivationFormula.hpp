#pragma once

#include <formula.hpp>

#include <QList>

namespace gurps_system {

/**
 * \brief Derivation descriptor: weighted sum of parent levels divided by a constant.
 *
 * Describes the algorithm:
 * \code
 *   result = (coefficients[0]*v[0] + ... + coefficients[n-1]*v[n-1]) / divisor
 * \endcode
 * (integer division, truncates toward zero)
 *
 * Examples:
 *   - Hit Points (HP = ST):          coefficients = {1}, divisor = 1
 *   - Basic Speed ((DX + HT) / 4):   coefficients = {1, 1}, divisor = 4
 *
 * The number of coefficients must match the number of \c ParentRef children
 * in the owning \c SecondaryAttribute.
 *
 * \pre \c divisor > 0
 * \pre \c coefficients is non-empty
 */
class ScaledSumDerivationFormula final : public Formula
{
  Q_OBJECT

  Q_PROPERTY(QList<int> coefficients READ coefficients WRITE setCoefficients
                 DESIGNABLE false SCRIPTABLE false)
  Q_PROPERTY(int divisor READ divisor WRITE setDivisor DESIGNABLE false SCRIPTABLE false)

public:
  /**
   * \param coefficients  Per-parent multiplicative weights.  Must be non-empty.
   * \param divisor       Applied to the weighted sum.  Must be > 0.
   * \param objectId      Optional stable identity.
   * \throws std::invalid_argument if \p divisor <= 0 or \p coefficients is empty.
   */
  explicit ScaledSumDerivationFormula(QList<int> coefficients, int divisor,
                                      boost::uuids::uuid objectId = boost::uuids::uuid{});

  // ── Identity ──────────────────────────────────────────────────────────────

  static auto classId() -> boost::uuids::uuid;
  auto typeId() const -> boost::uuids::uuid override;

  // ── Parameters ────────────────────────────────────────────────────────────

  auto coefficients() const -> QList<int>;

  /**
   * \brief Replaces the coefficient list.
   * \throws std::invalid_argument if \p value is empty.
   */
  auto setCoefficients(QList<int> value) -> void;

  auto divisor() const -> int;

  /**
   * \brief Sets the divisor.
   * \throws std::invalid_argument if \p value <= 0.
   */
  auto setDivisor(int value) -> void;

private:
  QList<int> _coefficients;
  int _divisor;
};

} // namespace gurps_system
