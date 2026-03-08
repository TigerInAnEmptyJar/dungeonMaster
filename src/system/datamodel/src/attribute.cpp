#include <attribute.hpp>
#include <formula.hpp>

#include <boost/uuid/string_generator.hpp>

#include <stdexcept>

namespace gurps_system {

// ── Impl ──────────────────────────────────────────────────────────────────────

struct Attribute::Impl
{
  // reserved for future per-instance state
};

// ── Helpers ───────────────────────────────────────────────────────────────────

static auto formulaChild(Attribute const& a) -> Formula const&
{
  if (a.size() == 0)
    throw std::logic_error{"Attribute: no formula child — add a formula before calling level()"};
  return static_cast<Formula const&>(*a.childAt(0));
}

// ── Construction ──────────────────────────────────────────────────────────────

Attribute::Attribute(boost::uuids::uuid objectId)
    : BaseObject{objectId}, _p{std::make_unique<Impl>()}
{
}

Attribute::~Attribute() = default;

// ── Identity ──────────────────────────────────────────────────────────────────

auto Attribute::classId() -> boost::uuids::uuid
{
  static auto const id = boost::uuids::string_generator{}("b3c4d5e6-f7a8-4b9c-0d1e-2f3a4b5c6d7e");
  return id;
}

auto Attribute::typeId() const -> boost::uuids::uuid { return classId(); }

// ── Computed ──────────────────────────────────────────────────────────────────

auto Attribute::level(int investedCp) const -> int
{
  return 10 + formulaChild(*this).levelBonus(investedCp);
}

auto Attribute::cpForLevelBonus(int levelBonus) const -> int
{
  return formulaChild(*this).cpCost(levelBonus);
}

} // namespace gurps_system
