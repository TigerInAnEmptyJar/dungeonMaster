#pragma once

#include "prerequisite.hpp"

namespace gurps_system {

/**
 * \brief Composite prerequisite where ALL child prerequisites must be satisfied (AND logic).
 *
 * This implements the Composite pattern for prerequisites, allowing complex
 * logical combinations. All children must be Prerequisite objects (which can
 * themselves be composite, enabling nested expressions).
 *
 * Example: Learning "Surgery" requires (IQ >= 12 AND "First Aid" >= 14)
 *
 * Child layout:
 *  N × \c Prerequisite objects (all must be satisfied)
 *
 * Use runtime \c dynamic_cast to identify child types and evaluate them.
 *
 * \note Objects must be heap-allocated via \c std::make_shared.
 */
class AndPrerequisite : public Prerequisite
{
  Q_OBJECT

public:
  explicit AndPrerequisite(boost::uuids::uuid objectId = boost::uuids::uuid{});
  ~AndPrerequisite() override;

  AndPrerequisite(AndPrerequisite const&) = delete;
  auto operator=(AndPrerequisite const&) -> AndPrerequisite& = delete;
  AndPrerequisite(AndPrerequisite&&) = delete;
  auto operator=(AndPrerequisite&&) -> AndPrerequisite& = delete;

  // ── Identity ──────────────────────────────────────────────────────────────

  static auto classId() -> boost::uuids::uuid;
  auto typeId() const -> boost::uuids::uuid override;

  // Note: All child management is inherited from TreeItem
  // Query children via childAt(i) and use dynamic_cast to determine type
};

/**
 * \brief Composite prerequisite where AT LEAST ONE child prerequisite must be satisfied (OR logic).
 *
 * This implements the Composite pattern for prerequisites, allowing complex
 * logical combinations. At least one child must be a Prerequisite, and at least
 * one must be satisfied for this prerequisite to be met.
 *
 * Example: Learning "Magic" requires (Magery advantage OR Cleric profession)
 *
 * Child layout:
 *  N × \c Prerequisite objects (at least one must be satisfied)
 *
 * Use runtime \c dynamic_cast to identify child types and evaluate them.
 *
 * \note Objects must be heap-allocated via \c std::make_shared.
 */
class OrPrerequisite : public Prerequisite
{
  Q_OBJECT

public:
  explicit OrPrerequisite(boost::uuids::uuid objectId = boost::uuids::uuid{});
  ~OrPrerequisite() override;

  OrPrerequisite(OrPrerequisite const&) = delete;
  auto operator=(OrPrerequisite const&) -> OrPrerequisite& = delete;
  OrPrerequisite(OrPrerequisite&&) = delete;
  auto operator=(OrPrerequisite&&) -> OrPrerequisite& = delete;

  // ── Identity ──────────────────────────────────────────────────────────────

  static auto classId() -> boost::uuids::uuid;
  auto typeId() const -> boost::uuids::uuid override;

  // Note: All child management is inherited from TreeItem
  // Query children via childAt(i) and use dynamic_cast to determine type
};

} // namespace gurps_system
