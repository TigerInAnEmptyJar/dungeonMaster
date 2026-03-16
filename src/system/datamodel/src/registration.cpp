#include <registration.hpp>

#include "profession.hpp"
#include <attribute.hpp>
#include <parentRef.hpp>
#include <race.hpp>
#include <skill.hpp>

#include "linearFormula.hpp"
#include "lookupDerivationFormula.hpp"
#include "lookupFormula.hpp"
#include "quadraticDerivationFormula.hpp"
#include "scaledSumDerivationFormula.hpp"

namespace gurps_system {

void registerSystemObjects(infrastructure::ObjectFactory& factory)
{
  factory.install(Attribute::classId(),
                  [](boost::uuids::uuid id) { return std::make_shared<Attribute>(id); });

  // costPerLevel=1 is the minimal valid value; overwritten by the serializer.
  factory.install(LinearFormula::classId(),
                  [](boost::uuids::uuid id) { return std::make_shared<LinearFormula>(10, id); });

  // {0,0} is the required sentinel entry; overwritten by the serializer.
  factory.install(LookupFormula::classId(), [](boost::uuids::uuid id) {
    return std::make_shared<LookupFormula>(std::map<int, int>{{0, 0}}, id);
  });

  factory.install(ParentRef::classId(),
                  [](boost::uuids::uuid id) { return std::make_shared<ParentRef>(id); });

  // coefficients={1}/divisor=1 is the minimal valid value; overwritten by the serializer.
  factory.install(ScaledSumDerivationFormula::classId(), [](boost::uuids::uuid id) {
    return std::make_shared<ScaledSumDerivationFormula>(QList<int>{1}, 1, id);
  });

  // divisor=1 is the minimal valid value; overwritten by the serializer.
  factory.install(QuadraticDerivationFormula::classId(), [](boost::uuids::uuid id) {
    return std::make_shared<QuadraticDerivationFormula>(5, id);
  });

  // {0,0} is the minimal valid entry; overwritten by the serializer.
  factory.install(LookupDerivationFormula::classId(), [](boost::uuids::uuid id) {
    return std::make_shared<LookupDerivationFormula>(std::map<int, int>{{0, 0}}, id);
  });

  factory.install(Race::classId(),
                  [](boost::uuids::uuid id) { return std::make_shared<Race>(id); });
  factory.install(Profession::classId(),
                  [](boost::uuids::uuid id) { return std::make_shared<Profession>(id); });
  factory.install(Skill::classId(),
                  [](boost::uuids::uuid id) { return std::make_shared<Skill>(id); });
}

auto formulaTypeToClassId(FormulaType type) -> boost::uuids::uuid
{
  switch (type) {
  case FormulaType::Linear:
    return LinearFormula::classId();
  case FormulaType::Lookup:
    return LookupFormula::classId();
  case FormulaType::ScaledSumDerivation:
    return ScaledSumDerivationFormula::classId();
  case FormulaType::QuadraticDerivation:
    return QuadraticDerivationFormula::classId();
  case FormulaType::LookupDerivation:
    return LookupDerivationFormula::classId();
  case FormulaType::None:
  default:
    return boost::uuids::uuid{}; // nil UUID
  }
}

} // namespace gurps_system
