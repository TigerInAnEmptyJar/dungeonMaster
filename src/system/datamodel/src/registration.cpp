#include <registration.hpp>

#include <attribute.hpp>
#include <linearFormula.hpp>
#include <lookupFormula.hpp>

namespace gurps_system {

void registerSystemObjects(infrastructure::ObjectFactory& factory)
{
  factory.install(Attribute::classId(),
                  [](boost::uuids::uuid id) { return std::make_shared<Attribute>(id); });

  // costPerLevel=1 is the minimal valid value; overwritten by the serializer.
  factory.install(LinearFormula::classId(),
                  [](boost::uuids::uuid id) { return std::make_shared<LinearFormula>(1, id); });

  // {0,0} is the required sentinel entry; overwritten by the serializer.
  factory.install(LookupFormula::classId(), [](boost::uuids::uuid id) {
    return std::make_shared<LookupFormula>(std::map<int, int>{{0, 0}}, id);
  });
}

} // namespace gurps_system
