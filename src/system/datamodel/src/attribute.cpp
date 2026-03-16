#include <attribute.hpp>

#include "derivationFormula.hpp"
#include "formula.hpp"
#include "parentRef.hpp"

#include <boost/uuid/string_generator.hpp>

#include <stdexcept>

namespace gurps_system {

// ── Construction ──────────────────────────────────────────────────────────────

Attribute::Attribute(boost::uuids::uuid objectId) : BaseObject{objectId} {}

Attribute::~Attribute() = default;

// ── Identity ──────────────────────────────────────────────────────────────────

auto Attribute::classId() -> boost::uuids::uuid
{
  static auto const id = boost::uuids::string_generator{}("b3c4d5e6-f7a8-4b9c-0d1e-2f3a4b5c6d7e");
  return id;
}

auto Attribute::typeId() const -> boost::uuids::uuid { return classId(); }

// ── Direct formula ────────────────────────────────────────────────────────────

auto Attribute::insertDirectFormula(std::shared_ptr<Formula> formula) -> void
{
  if (hasDirectFormula()) {
    throw std::logic_error{"Attribute: direct formula already present"};
  }
  insertChild(size(), std::move(formula));
}

auto Attribute::hasDirectFormula() const -> bool
{
  for (int i = 0; i < size(); ++i) {
    if (dynamic_cast<Formula*>(childAt(i).get()) != nullptr) {
      return true;
    }
  }
  return false;
}

auto Attribute::directFormula() const -> Formula const&
{
  for (int i = 0; i < size(); ++i) {
    if (auto* f = dynamic_cast<Formula*>(childAt(i).get())) {
      return *f;
    }
  }
  throw std::logic_error{"Attribute: no direct formula — call insertDirectFormula() first"};
}

// ── Derivation ────────────────────────────────────────────────────────────────

auto Attribute::insertDerivationFormula(std::shared_ptr<DerivationFormula> formula) -> void
{
  if (derivationFormula() != nullptr) {
    throw std::logic_error{"Attribute: derivation formula already present"};
  }
  insertChild(0, std::move(formula));
}

auto Attribute::derivationFormula() const -> DerivationFormula*
{
  if (size() == 0) {
    return nullptr;
  }
  return dynamic_cast<DerivationFormula*>(childAt(0).get());
}

auto Attribute::addParent(std::shared_ptr<ParentRef> ref) -> void
{
  // Insert before the first Formula child (if any); otherwise append.
  int pos = size();
  for (int i = 0; i < size(); ++i) {
    if (dynamic_cast<Formula*>(childAt(i).get()) != nullptr) {
      pos = i;
      break;
    }
  }
  insertChild(pos, std::move(ref));
}

auto Attribute::parentCount() const -> int
{
  int count = 0;
  for (int i = 0; i < size(); ++i) {
    if (dynamic_cast<ParentRef*>(childAt(i).get()) != nullptr) {
      ++count;
    }
  }
  return count;
}

auto Attribute::parentAt(int i) const -> ParentRef const&
{
  int count = 0;
  for (int j = 0; j < size(); ++j) {
    if (auto* pr = dynamic_cast<ParentRef*>(childAt(j).get())) {
      if (count == i) {
        return *pr;
      }
      ++count;
    }
  }
  throw std::out_of_range{"Attribute::parentAt: index out of range"};
}

} // namespace gurps_system
