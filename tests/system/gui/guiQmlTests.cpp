#include <QtQuickTest/quicktest.h>

#include "qmlTestSetup.hpp"

/**
 * \brief Qt Quick Test runner for GUI QML components.
 *
 * This runner automatically discovers and executes all QML test files
 * (tst_*.qml) in the same directory.
 *
 * Qt Quick Test provides a QML-based testing framework using TestCase elements.
 *
 * Note: Qt resources from the systemGui library should be automatically
 * registered when the library is linked.
 */

// Explicitly declare the resource initialization function
extern int qInitResources_qml();

// Force initialization by referencing the function in a global initializer
static int _resourceInit = qInitResources_qml();

QUICK_TEST_MAIN_WITH_SETUP(SystemGuiQmlTests, QmlTestSetup)
