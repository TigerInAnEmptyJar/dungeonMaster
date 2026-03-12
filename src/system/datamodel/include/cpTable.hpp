#pragma once

#include <map>

#include <QMetaType>

namespace gurps_system {

/**
 * \brief Lookup table mapping level bonus → cumulative character point cost.
 *
 * Used by \c LookupFormula to model GURPS 3rd edition non-linear costing.
 * Declared as a Qt metatype so the serialisers can reflect on Q_PROPERTY
 * declarations that expose this type.
 */
using CpTable = std::map<int, int>;

} // namespace gurps_system

Q_DECLARE_METATYPE(gurps_system::CpTable)
