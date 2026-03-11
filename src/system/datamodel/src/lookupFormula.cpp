#include <lookupFormula.hpp>

#include <boost/uuid/string_generator.hpp>

#include <stdexcept>

namespace gurps_system {

LookupFormula::LookupFormula(CpTable table, boost::uuids::uuid objectId)
    : Formula{objectId}, _table{std::move(table)}
{
  static bool const registered [[maybe_unused]] = [] {
    qRegisterMetaType<CpTable>();
    return true;
  }();

  auto it = _table.find(0);
  if (it == _table.end() || it->second != 0) {
    throw std::invalid_argument{"LookupFormula: table must contain entry {0, 0}"};
  }
}

auto LookupFormula::classId() -> boost::uuids::uuid
{
  static auto const id = boost::uuids::string_generator{}("d3e4f5a6-b7c8-4d9e-0f1a-2b3c4d5e6f7a");
  return id;
}

auto LookupFormula::typeId() const -> boost::uuids::uuid { return classId(); }

auto LookupFormula::levelBonus(int investedCp) const -> int
{
  int result = 0;
  for (auto const& [bonus, cost] : _table) {
    if (cost <= investedCp) {
      result = bonus;
    } else {
      break;
    }
  }
  return result;
}

auto LookupFormula::cpCost(int levelBonus) const -> int { return _table.at(levelBonus); }

auto LookupFormula::table() const -> CpTable const& { return _table; }

auto LookupFormula::setTable(CpTable table) -> void
{
  auto it = table.find(0);
  if (it == table.end() || it->second != 0) {
    throw std::invalid_argument{"LookupFormula: table must contain entry {0, 0}"};
  }
  _table = std::move(table);
}

} // namespace gurps_system
