#include <scaledSumDerivationFormula.hpp>

#include <boost/uuid/string_generator.hpp>

#include <QMetaType>

#include <stdexcept>

namespace gurps_system {

ScaledSumDerivationFormula::ScaledSumDerivationFormula(QList<int> coefficients, int divisor,
                                                       boost::uuids::uuid objectId)
    : DerivationFormula{objectId}, _coefficients{std::move(coefficients)}, _divisor{divisor}
{
  static bool const registered [[maybe_unused]] = [] {
    qRegisterMetaType<QList<int>>();
    return true;
  }();

  if (divisor <= 0) {
    throw std::invalid_argument{"ScaledSumDerivationFormula: divisor must be > 0"};
  }
  if (_coefficients.isEmpty()) {
    throw std::invalid_argument{"ScaledSumDerivationFormula: coefficients must be non-empty"};
  }
}

auto ScaledSumDerivationFormula::classId() -> boost::uuids::uuid
{
  static auto const id = boost::uuids::string_generator{}("a6b7c8d9-e0f1-4a2b-3c4d-5e6f7a8b9c0d");
  return id;
}

auto ScaledSumDerivationFormula::typeId() const -> boost::uuids::uuid { return classId(); }

auto ScaledSumDerivationFormula::coefficients() const -> QList<int> { return _coefficients; }

auto ScaledSumDerivationFormula::setCoefficients(QList<int> value) -> void
{
  if (value.isEmpty()) {
    throw std::invalid_argument{"ScaledSumDerivationFormula: coefficients must be non-empty"};
  }
  _coefficients = std::move(value);
}

auto ScaledSumDerivationFormula::divisor() const -> int { return _divisor; }

auto ScaledSumDerivationFormula::setDivisor(int value) -> void
{
  if (value <= 0) {
    throw std::invalid_argument{"ScaledSumDerivationFormula: divisor must be > 0"};
  }
  _divisor = value;
}

} // namespace gurps_system
