#include "attributeContainer.hpp"

#include <boost/uuid/string_generator.hpp>

namespace gurps_system {

AttributeContainer::AttributeContainer() : infrastructure::TreeItem() {}

boost::uuids::uuid const AttributeContainer::attributeContainerId =
    boost::uuids::string_generator{}("c35812f4-ca50-45c2-8e91-d95c95d32b59");

auto AttributeContainer::classId() -> boost::uuids::uuid
{
  static auto const id = boost::uuids::string_generator{}("f3ebbe4d-d9e5-4c76-b831-df44876fda42");
  return id;
}

auto AttributeContainer::typeId() const -> boost::uuids::uuid { return classId(); }

} // namespace gurps_system