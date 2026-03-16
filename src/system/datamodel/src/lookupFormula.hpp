#pragma once

#include <cpTable.hpp>
#include <formula.hpp>

namespace gurps_system {

/**
 * \brief GURPS 3rd edition formula: non-linear lookup table.
 *
 * The cost per level is not constant; instead an explicit mapping of
 * \c levelBonus → cumulative CP cost is provided at construction time.
 *
 * The table must satisfy:
 *   - Contains the entry {0, 0} (zero bonus costs zero CP).
 *   - Cumulative CP costs are strictly monotonically increasing with level bonus,
 *     so that the inverse lookup is unambiguous.
 *
 * \c levelBonus(cp) returns the highest level bonus whose cumulative cost does
 * not exceed \p cp.  Falls back to 0 if \p cp is below the first paid entry.
 *
 * \c cpCost(levelBonus) returns the exact cumulative cost for a level bonus present
 * in the table.  Throws \c std::out_of_range for unmapped values.
 *
 * Example (GURPS 3e style):
 * \code
 * LookupFormula formula({
 *     {0,  0},
 *     {1, 10},
 *     {2, 25},
 *     {3, 45},
 * });
 * \endcode
 */
class LookupFormula final : public Formula
{
  Q_OBJECT

  Q_PROPERTY(
      gurps_system::CpTable table READ table WRITE setTable DESIGNABLE false SCRIPTABLE false)

public:
  /**
   * \param table     Map of { levelBonus → cumulative CP cost }.
   *                  Must contain entry {0, 0}.
   * \param objectId  Optional stable identity; auto-generated when nil.
   *
   * \throws std::invalid_argument if the table does not contain entry {0, 0}.
   */
  explicit LookupFormula(std::map<int, int> table,
                         boost::uuids::uuid objectId = boost::uuids::uuid{});

  // ── Identity ──────────────────────────────────────────────────────────────

  static auto classId() -> boost::uuids::uuid;
  auto typeId() const -> boost::uuids::uuid override;

  // ── Formula ───────────────────────────────────────────────────────────────

  auto table() const -> CpTable const&;

  /**
   * \brief Sets the lookup table (used for deserialisation).
   *
   * \throws std::invalid_argument if the table does not contain entry {0, 0}.
   */
  auto setTable(CpTable table) -> void;

private:
  CpTable _table; // key: levelBonus, value: cumulative CP
};

} // namespace gurps_system
