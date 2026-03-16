#pragma once

#include "treeItem.hpp"

namespace gurps_system {

class RaceContainer : public virtual infrastructure::TreeItem
{
public:
  RaceContainer();
  RaceContainer(RaceContainer const&) = delete;
  auto operator=(RaceContainer const&) -> RaceContainer& = delete;
  RaceContainer(RaceContainer&&) = delete;
  auto operator=(RaceContainer&&) -> RaceContainer& = delete;
  virtual ~RaceContainer() = default;

  // ── Identity ──────────────────────────────────────────────────────────────

  static auto classId() -> boost::uuids::uuid;
  auto typeId() const -> boost::uuids::uuid override;

  static boost::uuids::uuid const raceContainerId;
};

} // namespace gurps_system
