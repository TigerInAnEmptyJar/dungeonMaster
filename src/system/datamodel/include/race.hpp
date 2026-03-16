#pragma once

#include <baseObject.hpp>

#include <memory>

namespace gurps_system {

/**
 * \brief A GURPS race definition.
 *
 * Stores the handbook description of a race: its name and description.
 * Races define the biological and cultural background of a character,
 * potentially including attribute modifiers, advantages, and disadvantages.
 *
 * \note Objects must be heap-allocated via \c std::make_shared.
 */
class Race : public gurps_system::BaseObject
{
  Q_OBJECT

public:
  explicit Race(boost::uuids::uuid objectId = boost::uuids::uuid{});
  ~Race() override;

  Race(Race const&) = delete;
  auto operator=(Race const&) -> Race& = delete;
  Race(Race&&) = delete;
  auto operator=(Race&&) -> Race& = delete;

  // ── Identity ──────────────────────────────────────────────────────────────

  static auto classId() -> boost::uuids::uuid;
  auto typeId() const -> boost::uuids::uuid override;
};

} // namespace gurps_system
