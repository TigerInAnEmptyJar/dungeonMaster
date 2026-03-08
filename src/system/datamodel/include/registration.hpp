#pragma once

#include <objectFactory.hpp>

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

} // namespace gurps_system
