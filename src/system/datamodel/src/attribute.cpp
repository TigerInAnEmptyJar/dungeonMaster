#include <attribute.hpp>

#include "derivationFormula.hpp"
#include "formula.hpp"
#include "parentRef.hpp"

#include <boost/uuid/string_generator.hpp>

#include <memory>
#include <stdexcept>

namespace gurps_system {

// ── Construction ──────────────────────────────────────────────────────────────

Attribute::Attribute(boost::uuids::uuid objectId) : BaseObject{objectId}
{
  connect(this, &Attribute::childInserted, this, [this](int index) {
    // Emit formulaChanged if the inserted child is a Formula (covers both direct and derivation
    // formulas)
    if (dynamic_cast<Formula*>(childAt(index).get())) {
      Q_EMIT formulaChanged();
    }
  });
}

Attribute::~Attribute() = default;

// ── Identity ──────────────────────────────────────────────────────────────────

auto Attribute::classId() -> boost::uuids::uuid
{
  static auto const id = boost::uuids::string_generator{}("b3c4d5e6-f7a8-4b9c-0d1e-2f3a4b5c6d7e");
  return id;
}

auto Attribute::typeId() const -> boost::uuids::uuid { return classId(); }

// ── Formula access ────────────────────────────────────────────────────────────

auto Attribute::formula() const -> infrastructure::TreeItem*
{
  // Check first child for formula (both direct Formula and DerivationFormula inherit from Formula)
  if (size() > 0) {
    if (auto f = std::dynamic_pointer_cast<Formula>(childAt(0))) {
      return f.get();
    }
  }

  return nullptr;
}

auto Attribute::setFormula(infrastructure::TreeItem* aFormula) -> void
{
  if (aFormula && !dynamic_cast<Formula*>(aFormula)) {
    throw std::invalid_argument{"setFormula: provided object does not inherit from Formula"};
  }

  if (size() > 0) {
    // Replace or remove existing formula
    if (aFormula) {
      this->swapChild(childAt(0), aFormula->shared_from_this());
    } else {
      // Remove the formula
      this->removeChild(childAt(0));
    }
  } else if (aFormula) {
    // Insert new formula at position 0
    insertChild(0, aFormula->shared_from_this());
  }

  Q_EMIT formulaChanged();
}

} // namespace gurps_system
