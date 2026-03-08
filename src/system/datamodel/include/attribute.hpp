#pragma once

#include <baseObject.hpp>

#include <memory>

namespace gurps_system {

/**
 * \brief A GURPS attribute definition, such as Strength or Dexterity.
 *
 * An Attribute is the handbook description of an attribute: its name,
 * description, and the formula that maps CP investment to level bonus.
 * It holds no per-character state.
 *
 * Attributes must be heap-allocated and owned by a \c shared_ptr when a
 * formula child is inserted (because \c insertChild uses \c shared_from_this()).
 * Construct via \c std::make_shared and insert the formula child afterwards:
 * \code
 *   auto st = std::make_shared<Attribute>();
 *   st->insertChild(0, std::make_shared<LinearFormula>(10));
 *   int lvl = st->level(investedCp);
 * \endcode
 * The base level is always 10.  Negative CP investment produces values below 10.
 *
 * \throws std::logic_error from \c level() and \c cpForLevelBonus() when
 *         the formula child has not yet been added.
 *
 * Per-character state (invested CP, live level value) is held by
 * \c InstanceAttribute, which references an \c Attribute for recalculation
 * and documentation.
 *
 * The formula is an implementation detail: it is stored as the first tree
 * child of the Attribute and is not accessible through the public API.
 */
class Attribute : public gurps_system::BaseObject
{
  Q_OBJECT

public:
  /**
   * \brief Constructs an initially formula-free Attribute.
   *
   * The formula must be added as the first tree child (index 0) before
   * calling \c level() or \c cpForLevelBonus().
   *
   * \param objectId  Optional stable identity; auto-generated when nil.
   */
  explicit Attribute(boost::uuids::uuid objectId = boost::uuids::uuid{});
  ~Attribute() override;

  Attribute(Attribute const&) = delete;
  auto operator=(Attribute const&) -> Attribute& = delete;
  Attribute(Attribute&&) = delete;
  auto operator=(Attribute&&) -> Attribute& = delete;

  // ── Identity ──────────────────────────────────────────────────────────────

  static auto classId() -> boost::uuids::uuid;
  auto typeId() const -> boost::uuids::uuid override;

  // ── Computed ──────────────────────────────────────────────────────────────

  /**
   * \brief Computes the effective level for a given CP investment.
   *
   * \param investedCp  Character points spent on this attribute.
   *                    May be negative to reach levels below 10.
   * \return            10 + formula.levelBonus(investedCp)
   * \throws std::logic_error if no formula child has been added.
   */
  auto level(int investedCp) const -> int;

  /**
   * \brief CP cost to achieve the given level bonus, via the formula.
   *
   * \throws std::logic_error if no formula child has been added.
   */
  auto cpForLevelBonus(int levelBonus) const -> int;

private:
  struct Impl;
  std::unique_ptr<Impl> _p;
};

} // namespace gurps_system
