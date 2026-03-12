#include <quadraticDerivationFormula.hpp>

#include <boost/uuid/string_generator.hpp>

#include <stdexcept>

namespace gurps_system {

QuadraticDerivationFormula::QuadraticDerivationFormula(int divisor, boost::uuids::uuid objectId)
    : DerivationFormula{objectId}, _divisor{divisor}
{
  if (divisor <= 0) {
    throw std::invalid_argument{"QuadraticDerivationFormula: divisor must be > 0"};
  }
}

auto QuadraticDerivationFormula::classId() -> boost::uuids::uuid
{
  static auto const id = boost::uuids::string_generator{}("b7c8d9e0-f1a2-4b3c-4d5e-6f7a8b9c0d1e");
  return id;
}

auto QuadraticDerivationFormula::typeId() const -> boost::uuids::uuid { return classId(); }

auto QuadraticDerivationFormula::divisor() const -> int { return _divisor; }

auto QuadraticDerivationFormula::setDivisor(int value) -> void
{
  if (value <= 0) {
    throw std::invalid_argument{"QuadraticDerivationFormula: divisor must be > 0"};
  }
  _divisor = value;
}

} // namespace gurps_system
