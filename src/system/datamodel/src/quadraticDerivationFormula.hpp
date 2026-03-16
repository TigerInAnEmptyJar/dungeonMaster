#pragma once

#include <formula.hpp>

namespace gurps_system {

/**
 * \brief Derivation descriptor: single parent level squared, divided by a constant.
 *
 * Describes the algorithm:
 * \code
 *   result = v[0] * v[0] / divisor
 * \endcode
 * (integer division, truncates toward zero)
 *
 * Example:
 *   - Basic Lift (BL = ST² / 5):  divisor = 5
 *
 * The owning \c SecondaryAttribute must have exactly one \c ParentRef child.
 *
 * \pre \c divisor > 0
 */
class QuadraticDerivationFormula final : public Formula
{
  Q_OBJECT

  Q_PROPERTY(int divisor READ divisor WRITE setDivisor DESIGNABLE false SCRIPTABLE false)

public:
  /**
   * \param divisor   Applied to the squared parent level.  Must be > 0.
   * \param objectId  Optional stable identity.
   * \throws std::invalid_argument if \p divisor <= 0.
   */
  explicit QuadraticDerivationFormula(int divisor,
                                      boost::uuids::uuid objectId = boost::uuids::uuid{});

  // ── Identity ──────────────────────────────────────────────────────────────

  static auto classId() -> boost::uuids::uuid;
  auto typeId() const -> boost::uuids::uuid override;

  // ── Parameters ────────────────────────────────────────────────────────────

  auto divisor() const -> int;

  /**
   * \brief Sets the divisor.
   * \throws std::invalid_argument if \p value <= 0.
   */
  auto setDivisor(int value) -> void;

private:
  int _divisor = 5;
};

} // namespace gurps_system
