#pragma once

#include <cpTable.hpp>
#include <derivationFormula.hpp>

namespace gurps_system {

/**
 * \brief Derivation descriptor: direct table lookup from a single parent level.
 *
 * Stores an explicit mapping of \c parentLevel → \c derivedLevel.
 *
 * Example:
 *   - Damage (Thrust / Swing) from ST: non-linear table keyed by ST level
 *
 * The owning \c SecondaryAttribute must have exactly one \c ParentRef child.
 *
 * Reuses \c CpTable (\c std::map<int,int>) as the storage type; the key is the
 * parent level and the value is the derived level.  Unlike \c LookupFormula,
 * there is no requirement for a \c {0,0} sentinel entry.
 *
 * \pre The table must be non-empty.
 */
class LookupDerivationFormula final : public DerivationFormula
{
  Q_OBJECT

  Q_PROPERTY(
      gurps_system::CpTable table READ table WRITE setTable DESIGNABLE false SCRIPTABLE false)

public:
  /**
   * \param table     Map of { parentLevel → derivedLevel }.  Must be non-empty.
   * \param objectId  Optional stable identity.
   * \throws std::invalid_argument if \p table is empty.
   */
  explicit LookupDerivationFormula(CpTable table,
                                   boost::uuids::uuid objectId = boost::uuids::uuid{});

  // ── Identity ──────────────────────────────────────────────────────────────

  static auto classId() -> boost::uuids::uuid;
  auto typeId() const -> boost::uuids::uuid override;

  // ── Parameters ────────────────────────────────────────────────────────────

  auto table() const -> CpTable const&;

  /**
   * \brief Replaces the lookup table.
   * \throws std::invalid_argument if \p value is empty.
   */
  auto setTable(CpTable value) -> void;

private:
  CpTable _table;
};

} // namespace gurps_system
