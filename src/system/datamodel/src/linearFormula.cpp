#include <linearFormula.hpp>

#include <boost/uuid/string_generator.hpp>

#include <stdexcept>

namespace gurps_system {

LinearFormula::LinearFormula(int costPerLevel, boost::uuids::uuid objectId)
    : Formula{objectId}, _costPerLevel{costPerLevel}
{
  if (costPerLevel <= 0) {
    throw std::invalid_argument{"LinearFormula: costPerLevel must be > 0"};
  }
}

auto LinearFormula::classId() -> boost::uuids::uuid
{
  static auto const id = boost::uuids::string_generator{}("c2d3e4f5-a6b7-4c8d-9e0f-1a2b3c4d5e6f");
  return id;
}

auto LinearFormula::typeId() const -> boost::uuids::uuid { return classId(); }

auto LinearFormula::costPerLevel() const -> int { return _costPerLevel; }

auto LinearFormula::setCostPerLevel(int value) -> void
{
  if (value <= 0) {
    throw std::invalid_argument{"LinearFormula: costPerLevel must be > 0"};
  }
  if (_costPerLevel == value) {
    return;
  }
  _costPerLevel = value;
}

} // namespace gurps_system
