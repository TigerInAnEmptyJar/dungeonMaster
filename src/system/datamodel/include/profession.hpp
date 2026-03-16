#pragma once

#include <baseObject.hpp>

#include <memory>

namespace gurps_system {

/**
 * \brief A GURPS profession definition.
 *
 * Stores the handbook description of a profession: its name and description.
 * Professions can be used to categorize characters and provide templates
 * for skill sets and attribute suggestions.
 *
 * \note Objects must be heap-allocated via \c std::make_shared.
 */
class Profession : public gurps_system::BaseObject
{
  Q_OBJECT

public:
  explicit Profession(boost::uuids::uuid objectId = boost::uuids::uuid{});
  ~Profession() override;

  Profession(Profession const&) = delete;
  auto operator=(Profession const&) -> Profession& = delete;
  Profession(Profession&&) = delete;
  auto operator=(Profession&&) -> Profession& = delete;

  // ── Identity ──────────────────────────────────────────────────────────────

  static auto classId() -> boost::uuids::uuid;
  auto typeId() const -> boost::uuids::uuid override;
};

} // namespace gurps_system
