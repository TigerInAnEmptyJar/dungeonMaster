#pragma once

#include <objectFactory.hpp>

#include <boost/uuid/uuid.hpp>

namespace gurps_system {

/**
 * \brief Registers all system datamodel types with the given factory.
 *
 * Must be called once at application startup (or in tests that exercise
 * deserialization) before any \c ObjectFactory::create() call for a
 * system datamodel type.
 *
 * Currently registered types:
 *  - \c Attribute      (classId: b3c4d5e6-...)
 *  - \c LinearFormula  (classId: ...)
 *  - \c LookupFormula  (classId: ...)
 *
 * \param factory  The factory to install creators into.
 */
void registerSystemObjects(infrastructure::ObjectFactory& factory);

/**
 * \brief Enumeration of formula types for GUI/user interaction.
 */
enum class FormulaType
{
  None = 0,
  Linear = 1,
  Lookup = 2,
  ScaledSumDerivation = 3,
  QuadraticDerivation = 4,
  LookupDerivation = 5
};

/**
 * \brief Returns the classId UUID for a given formula type.
 *
 * \param type  The formula type.
 * \returns The corresponding classId, or nil UUID for FormulaType::None.
 */
auto formulaTypeToClassId(FormulaType type) -> boost::uuids::uuid;

} // namespace gurps_system
