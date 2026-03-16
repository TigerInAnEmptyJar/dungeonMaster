#include <formula.hpp>

#include <boost/uuid/string_generator.hpp>

namespace gurps_system {

Formula::Formula(boost::uuids::uuid objectId) : infrastructure::TreeItem{objectId} {}

auto Formula::maxDirectBonus() const -> int { return _maxDirectBonus; }

auto Formula::setMaxDirectBonus(int value) -> void
{
  if (_maxDirectBonus == value) {
    return;
  }
  _maxDirectBonus = value;
  emit maxDirectBonusChanged(value);
}

auto Formula::classId() -> boost::uuids::uuid
{
  static auto const id = boost::uuids::string_generator{}("a1b2c3d4-e5f6-4a7b-8c9d-0e1f2a3b4c5d");
  return id;
}

} // namespace gurps_system
