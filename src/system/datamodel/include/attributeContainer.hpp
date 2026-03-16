#pragma once

#include "treeItem.hpp"

namespace gurps_system {

class AttributeContainer : public virtual infrastructure::TreeItem
{
public:
  AttributeContainer();
  AttributeContainer(AttributeContainer const&) = delete;
  auto operator=(AttributeContainer const&) -> AttributeContainer& = delete;
  AttributeContainer(AttributeContainer&&) = delete;
  auto operator=(AttributeContainer&&) -> AttributeContainer& = delete;
  virtual ~AttributeContainer() = default;

  // ── Identity ──────────────────────────────────────────────────────────────

  static auto classId() -> boost::uuids::uuid;
  auto typeId() const -> boost::uuids::uuid override;

  static boost::uuids::uuid const attributeContainerId;
};

} // namespace gurps_system