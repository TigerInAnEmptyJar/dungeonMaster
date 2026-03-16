#include <registration.hpp>

#include "profession.hpp"
#include <attribute.hpp>
#include <parentRef.hpp>
#include <race.hpp>
#include <skill.hpp>

#include "attributeSkillPrerequisite.hpp"
#include "compositePrerequisites.hpp"
#include "linearFormula.hpp"
#include "lookupDerivationFormula.hpp"
#include "lookupFormula.hpp"
#include "prerequisite.hpp"
#include "quadraticDerivationFormula.hpp"
#include "scaledSumDerivationFormula.hpp"
#include "simplePrerequisites.hpp"
#include "skillDefault.hpp"

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

  // Skill defaults and prerequisites
  factory.install(SkillDefault::classId(),
                  [](boost::uuids::uuid id) { return std::make_shared<SkillDefault>(id); });
  factory.install(AttributePrerequisite::classId(), [](boost::uuids::uuid id) {
    return std::make_shared<AttributePrerequisite>(id);
  });
  factory.install(SkillPrerequisite::classId(),
                  [](boost::uuids::uuid id) { return std::make_shared<SkillPrerequisite>(id); });
  factory.install(AdvantagePrerequisite::classId(), [](boost::uuids::uuid id) {
    return std::make_shared<AdvantagePrerequisite>(id);
  });
  factory.install(ProfessionPrerequisite::classId(), [](boost::uuids::uuid id) {
    return std::make_shared<ProfessionPrerequisite>(id);
  });
  factory.install(RacePrerequisite::classId(),
                  [](boost::uuids::uuid id) { return std::make_shared<RacePrerequisite>(id); });
  factory.install(AndPrerequisite::classId(),
                  [](boost::uuids::uuid id) { return std::make_shared<AndPrerequisite>(id); });
  factory.install(OrPrerequisite::classId(),
                  [](boost::uuids::uuid id) { return std::make_shared<OrPrerequisite>(id); });
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
