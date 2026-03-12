#include <secondaryAttribute.hpp>

#include "derivationFormula.hpp"
#include "formula.hpp"
#include "parentRef.hpp"

#include <boost/uuid/string_generator.hpp>

#include <stdexcept>

namespace gurps_system {

namespace {
// {d9e0f1a2-b3c4-4d5e-6f7a-8b9c0d1e2f3a}
static auto k_classId = boost::uuids::string_generator{}("d9e0f1a2-b3c4-4d5e-6f7a-8b9c0d1e2f3a");
} // anonymous namespace

// ── Construction ──────────────────────────────────────────────────────────────

SecondaryAttribute::SecondaryAttribute(boost::uuids::uuid objectId) : BaseObject{objectId} {}

SecondaryAttribute::~SecondaryAttribute() = default;

// ── Identity ──────────────────────────────────────────────────────────────────

auto SecondaryAttribute::classId() -> boost::uuids::uuid { return k_classId; }

auto SecondaryAttribute::typeId() const -> boost::uuids::uuid { return k_classId; }

// ── Properties ────────────────────────────────────────────────────────────────

auto SecondaryAttribute::maxDirectBonus() const -> int { return _maxDirectBonus; }

auto SecondaryAttribute::setMaxDirectBonus(int value) -> void
{
  if (_maxDirectBonus == value) {
    return;
  }
  _maxDirectBonus = value;
  emit maxDirectBonusChanged(value);
}

// ── Derivation setup ──────────────────────────────────────────────────────────

auto SecondaryAttribute::insertDerivationFormula(std::shared_ptr<DerivationFormula> formula) -> void
{
  if (derivationFormula() != nullptr) {
    throw std::logic_error{"SecondaryAttribute: derivation formula already present"};
  }
  insertChild(0, std::move(formula));
}

auto SecondaryAttribute::addParent(std::shared_ptr<ParentRef> ref) -> void
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

auto SecondaryAttribute::insertDirectFormula(std::shared_ptr<Formula> formula) -> void
{
  if (hasDirectFormula()) {
    throw std::logic_error{"SecondaryAttribute: direct formula already present"};
  }
  insertChild(size(), std::move(formula));
}

// ── Accessors ─────────────────────────────────────────────────────────────────

auto SecondaryAttribute::derivationFormula() const -> DerivationFormula*
{
  if (size() == 0) {
    return nullptr;
  }
  return dynamic_cast<DerivationFormula*>(childAt(0).get());
}

auto SecondaryAttribute::parentCount() const -> int
{
  int count = 0;
  for (int i = 0; i < size(); ++i) {
    if (dynamic_cast<ParentRef*>(childAt(i).get()) != nullptr) {
      ++count;
    }
  }
  return count;
}

auto SecondaryAttribute::parentAt(int i) const -> ParentRef const&
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
  throw std::out_of_range{"SecondaryAttribute::parentAt: index out of range"};
}

auto SecondaryAttribute::hasDirectFormula() const -> bool
{
  for (int i = 0; i < size(); ++i) {
    if (dynamic_cast<Formula*>(childAt(i).get()) != nullptr) {
      return true;
    }
  }
  return false;
}

auto SecondaryAttribute::directFormula() const -> Formula const&
{
  for (int i = 0; i < size(); ++i) {
    if (auto* f = dynamic_cast<Formula*>(childAt(i).get())) {
      return *f;
    }
  }
  throw std::logic_error{
      "SecondaryAttribute: no direct formula — add one before calling directFormula()"};
}

} // namespace gurps_system
