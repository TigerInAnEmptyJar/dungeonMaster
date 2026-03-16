#pragma once

#include "treeItem.hpp"

namespace gurps_system {

class ProfessionContainer : public virtual infrastructure::TreeItem
{
public:
  ProfessionContainer();
  ProfessionContainer(ProfessionContainer const&) = delete;
  auto operator=(ProfessionContainer const&) -> ProfessionContainer& = delete;
  ProfessionContainer(ProfessionContainer&&) = delete;
  auto operator=(ProfessionContainer&&) -> ProfessionContainer& = delete;
  virtual ~ProfessionContainer() = default;

  // ── Identity ──────────────────────────────────────────────────────────────

  static auto classId() -> boost::uuids::uuid;
  auto typeId() const -> boost::uuids::uuid override;

  static boost::uuids::uuid const professionContainerId;
};

} // namespace gurps_system
