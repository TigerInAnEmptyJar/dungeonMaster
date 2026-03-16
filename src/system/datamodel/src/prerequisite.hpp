#pragma once

#include <treeItem.hpp>

namespace gurps_system {

/**
 * \brief Abstract base class for skill prerequisites.
 *
 * Prerequisites define requirements that must be met before a skill can be learned.
 * This follows the Composite pattern similar to the Formula hierarchy, allowing
 * simple prerequisites (attribute, skill, advantage, profession, race) and
 * composite prerequisites (AND, OR) to be combined into complex requirement trees.
 *
 * Prerequisites are stored as children of the Skill object.
 *
 * Runtime type identification via \c dynamic_cast determines the concrete
 * prerequisite type and how to evaluate it.
 *
 * \note Objects must be heap-allocated via \c std::make_shared.
 */
class Prerequisite : public infrastructure::TreeItem
{
  Q_OBJECT

public:
  explicit Prerequisite(boost::uuids::uuid objectId = boost::uuids::uuid{});
  ~Prerequisite() override;

  Prerequisite(Prerequisite const&) = delete;
  auto operator=(Prerequisite const&) -> Prerequisite& = delete;
  Prerequisite(Prerequisite&&) = delete;
  auto operator=(Prerequisite&&) -> Prerequisite& = delete;

  // ── Identity ──────────────────────────────────────────────────────────────

  static auto classId() -> boost::uuids::uuid;
  auto typeId() const -> boost::uuids::uuid override;
};

} // namespace gurps_system
