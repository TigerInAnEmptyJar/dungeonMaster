#include "lookupDerivationFormula.hpp"

#include <boost/uuid/string_generator.hpp>

#include <stdexcept>

namespace gurps_system {

// ── Construction ──────────────────────────────────────────────────────────────

LookupDerivationFormula::LookupDerivationFormula(CpTable table, boost::uuids::uuid objectId)
    : Formula(objectId), _table(std::move(table))
{
  if (_table.empty()) {
    throw std::invalid_argument{"LookupDerivationFormula: table must not be empty"};
  }
}

// ── Identity ──────────────────────────────────────────────────────────────────

auto LookupDerivationFormula::classId() -> boost::uuids::uuid
{
  static auto const id = boost::uuids::string_generator{}("c8d9e0f1-a2b3-4c4d-5e6f-7a8b9c0d1e2f");
  return id;
}

auto LookupDerivationFormula::typeId() const -> boost::uuids::uuid { return classId(); }

// ── Parameters ────────────────────────────────────────────────────────────────

auto LookupDerivationFormula::table() const -> CpTable const& { return _table; }

auto LookupDerivationFormula::setTable(CpTable value) -> void
{
  if (value.empty()) {
    throw std::invalid_argument{"LookupDerivationFormula: table must not be empty"};
  }
  _table = std::move(value);
}

} // namespace gurps_system
