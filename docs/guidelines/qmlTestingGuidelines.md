# QML Testing Guidelines

This document describes best practices for testing QML components and Qt models in this project.

## Test Types

### C++ Model Tests (GTest)

Use GTest for testing `QAbstractItemModel` subclasses and Qt C++ code:

**Purpose:**
- Model initialization and configuration
- Data access through roles
- Dynamic updates (insertion/removal)
- Signal emission and connections
- Business logic in C++ classes

**Location:** `tests/*/gui/*Test.cpp`

### QML Component Tests (Qt Quick Test)

Use Qt Quick Test for testing QML UI components:

**Purpose:**
- Component creation and initialization
- Property bindings and data flow
- Model-view integration
- User interactions (clicks, keyboard input)
- Visual layout and sizing

**Location:** `tests/*/gui/tst_*.qml`

## Writing C++ Model Tests

### Basic Test Structure

```cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <myListModel.hpp>

class MyListModelTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_model = std::make_unique<MyListModel>();
        m_container = std::make_shared<MyContainer>();
    }
    
    std::unique_ptr<MyListModel> m_model;
    std::shared_ptr<MyContainer> m_container;
};

TEST_F(MyListModelTest, InitiallyEmpty) {
    EXPECT_EQ(m_model->rowCount(), 0);
}
```

### Testing Roles

```cpp
TEST_F(MyListModelTest, ProvidesCorrectRoles) {
    m_model->setContainer(m_container.get());
    
    // Test role names
    auto roles = m_model->roleNames();
    EXPECT_TRUE(roles.contains(MyListModel::NameRole));
    EXPECT_EQ(roles[MyListModel::NameRole], "name");
    
    // Test data access
    auto index = m_model->index(0, 0);
    auto name = m_model->data(index, MyListModel::NameRole);
    EXPECT_EQ(name.toString(), "Expected Name");
}
```

### Testing Signals with GMock

Create a mock receiver for more powerful signal testing:

```cpp
// mockModelReceiver.hpp
class MockModelReceiver {
public:
    MOCK_METHOD(void, onDataChanged, 
                (const QModelIndex&, const QModelIndex&, const QList<int>&),
                ());
    MOCK_METHOD(void, onRowsInserted,
                (const QModelIndex&, int, int),
                ());
    MockModelReceiver(QAbstractItemModel& model)
    {
        QObject::connect(m_model.get(), &QAbstractItemModel::dataChanged,
                         [this](auto a, auto b, auto c){onDataChanged(a, b, c);});
        QObject::connect(m_model.get(), &QAbstractItemModel::rowsInserted,
                         [this](auto a, auto b, auto c){onRowsInserted(a, b, c);});
    }
};

// In test
TEST_F(MyListModelTest, EmitsSignalsOnDataChange) {
    MockModelReceiver receiver(model);
    
    using ::testing::_;
    EXPECT_CALL(receiver, onDataChanged(_, _, _)).Times(1);
    
    m_container->updateItem(0);  // Trigger change
}
```

### Testing Dynamic Updates

```cpp
TEST_F(MyListModelTest, UpdatesOnItemInsertion) {
    m_model->setContainer(m_container.get());
    int initialCount = m_model->rowCount();
    
    m_container->addItem(newItem);
    
    EXPECT_EQ(m_model->rowCount(), initialCount + 1);
}
```

## Writing QML Component Tests

### Basic Test Structure

```qml
import QtQuick
import QtTest

TestCase {
    id: testCase
    name: "MyComponentTests"
    
    // Component to test
    Component {
        id: testComponent
        MyComponent {
            width: 400
            height: 300
        }
    }
    
    function test_componentCreation() {
        var component = createTemporaryObject(testComponent, testCase)
        verify(component !== null, "Component should be created")
    }
}
```

### Testing with Mock Data

Create simple mock objects for testing:

```qml
TestCase {
    name: "MyListViewTests"
    
    // Mock data model
    ListModel {
        id: mockModel
        ListElement { name: "Item 1"; description: "First item" }
        ListElement { name: "Item 2"; description: "Second item" }
    }
    
    // Mock object
    QtObject {
        id: mockItem
        property string name: "Test Item"
        property string description: "Test description"
    }
    
    function test_displaysModel() {
        var view = createTemporaryObject(testComponent, testCase)
        view.model = mockModel
        waitForRendering(view)
        verify(view.count === 2, "Should have 2 items")
    }
}
```

### Loading Production Components from Resources

For integration tests, load actual production components:

```qml
TestCase {
    name: "MyComponentIntegrationTests"
    
    // Load actual component from Qt resources
    Loader {
        id: componentLoader
        source: "qrc:/qml/myModule/qml/MyComponent.qml"
        asynchronous: false
    }
    
    function test_componentLoadsSuccessfully() {
        compare(componentLoader.status, Loader.Ready, 
                "Component should load from resources")
        verify(componentLoader.item !== null, 
               "Component item should be created")
    }
    
    function test_componentWithRealData() {
        var component = componentLoader.item
        component.item = mockItem
        waitForRendering(component)
        verify(component.item !== null, "Item should be set")
    }
}
```

### Testing Interactions

```qml
function test_clickEmitsSignal() {
    var component = createTemporaryObject(testComponent, testCase)
    var signalSpy = signalSpy.createObject(testCase, {
        target: component,
        signalName: "itemSelected"
    })
    
    mouseClick(component, 10, 10)
    
    compare(signalSpy.count, 1, "Signal should be emitted once")
}

Component {
    id: signalSpyComponent
    SignalSpy {}
}
```

### Testing Property Bindings

```qml
function test_propertyBinding() {
    var component = createTemporaryObject(testComponent, testCase)
    component.myProperty = "test value"
    compare(component.myProperty, "test value", "Property should be set")
}

function test_twoWayBinding() {
    var parent = createTemporaryQmlObject('
        import QtQuick
        Item {
            property string sharedValue: "initial"
            MyComponent {
                id: child
                value: parent.sharedValue
            }
        }
    ', testCase)
    
    compare(parent.child.value, "initial")
    parent.sharedValue = "updated"
    compare(parent.child.value, "updated")
}
```

### Testing with Models

```qml
function test_modelIntegration() {
    var component = createTemporaryObject(testComponent, testCase)
    component.model = mockModel
    
    waitForRendering(component)
    
    compare(component.count, mockModel.count, "Counts should match")
}
```

## CMake Integration

### C++ Tests

Use `my_add_test` for GTest-based tests:

```cmake
my_add_test(MyGuiModelTests
  AUTOMOC
  DEPENDS
    DM::MyModule::Gui
    Qt6::Core
    Qt6::Test
  SOURCE
    myListModelTest.cpp
)
```

### QML Tests

Use `my_add_qml_test` for Qt Quick Test:

```cmake
my_add_qml_test(MyGuiQmlTests
  SOURCE
    guiQmlTests.cpp
  RESOURCES
    tst_myListView.qml
    tst_myDetailView.qml
  DEPENDS
    DM::MyModule::Gui
)
```

### QML Test Runner

The C++ test runner must use `QUICK_TEST_MAIN`:

```cpp
// guiQmlTests.cpp
#include <QtQuickTest>

// For static libraries, initialize resources
extern int qInitResources_qml();
static int _resourceInit = qInitResources_qml();

QUICK_TEST_MAIN(MyGuiQmlTests)
```

## Test Organization

### File Structure

```
tests/myModule/gui/
├── CMakeLists.txt
├── myListModelTest.cpp         # C++ model tests
├── mockModelReceiver.hpp        # Mock objects for C++ tests
├── guiQmlTests.cpp              # QML test runner
├── tst_myListView.qml          # QML component tests
└── tst_myDetailView.qml        # QML component tests
```

### Naming Conventions

- **C++ test files**: `*Test.cpp` (e.g., `myListModelTest.cpp`)
- **QML test files**: `tst_*.qml` (e.g., `tst_myListView.qml`)
- **Test case names**: Descriptive, use TitleCase
- **Test functions**: Start with `test_`, use snake_case

## Qt Quick Test API Reference

### Essential Functions

| Function | Purpose |
|----------|---------|
| `verify(condition, msg)` | Assert condition is true |
| `compare(actual, expected, msg)` | Assert equality |
| `fuzzyCompare(actual, expected, delta, msg)` | Compare floats with tolerance |
| `tryCompare(obj, property, expected, timeout, msg)` | Wait for property to reach value |
| `expectFail(tag, msg)` | Mark next assertion as expected failure |

### Object Creation

| Function | Purpose |
|----------|---------|
| `createTemporaryObject(component, parent, properties)` | Create object, auto-cleanup |
| `createTemporaryQmlObject(qml, parent, filePath)` | Create from QML string |
| `findChild(parent, objectName)` | Find child by objectName |

### Timing and Waiting

| Function | Purpose |
|----------|---------|
| `wait(ms)` | Wait specified milliseconds |
| `waitForRendering(item, timeout)` | Wait for rendering |
| `tryVerify(fn, timeout, msg)` | Retry verification until timeout |

### Input Simulation

| Function | Purpose |
|----------|---------|
| `mousePress(item, x, y, button, modifiers, delay)` | Press mouse button |
| `mouseRelease(item, x, y, button, modifiers, delay)` | Release mouse button |
| `mouseClick(item, x, y, button, modifiers, delay)` | Click mouse |
| `mouseDoubleClick(item, x, y, button, modifiers, delay)` | Double-click |
| `mouseMove(item, x, y, delay)` | Move mouse |
| `keyPress(key, modifiers, delay)` | Press key |
| `keyRelease(key, modifiers, delay)` | Release key |
| `keyClick(key, modifiers, delay)` | Press and release key |

## Best Practices

### Test Independence

1. **Isolation**: Each test should be completely independent
2. **Cleanup**: Use `createTemporaryObject()` for automatic cleanup
3. **Fresh State**: Create new objects in each test, don't reuse
4. **Setup/Teardown**: Use `init()` and `cleanup()` functions if needed

### Mock Data

1. **Simplicity**: Use minimal mock data sufficient for the test
2. **Reusability**: Define mock objects once, reuse across tests
3. **Clarity**: Make mock data obvious and descriptive
4. **Avoid Production Dependencies**: Don't load full production data in unit tests

### Assertions

1. **Descriptive Messages**: Always provide clear failure messages
2. **One Concept**: Test one thing per test function
3. **Appropriate Matchers**: Use the most specific assertion available
4. **Expected First**: In `compare()`, put expected value first by convention

### Integration Tests

1. **Unit First**: Write unit tests with mocks before integration tests
2. **Load Real Components**: Use `Loader` with `qrc:/` URLs for real components
3. **Resource Paths**: Match paths exactly from `.qrc` file structure
4. **Verify Loading**: Always check that resources load successfully

### Performance

1. **Fast Tests**: Keep tests fast; mock external dependencies
2. **Parallel Safe**: Tests should be safe to run in parallel
3. **No Sleeps**: Avoid `wait()` when possible; use `tryCompare()` instead
4. **Offscreen**: Use `QT_QPA_PLATFORM=offscreen` to avoid display requirements

## Common Testing Patterns

### Testing Lifecycle

```qml
function init() {
    // Called before each test
}

function cleanup() {
    // Called after each test
}

function initTestCase() {
    // Called once before all tests
}

function cleanupTestCase() {
    // Called once after all tests
}
```

### Testing Errors

```qml
function test_errorHandling() {
    var component = createTemporaryObject(testComponent, testCase)
    
    ignoreWarning("Expected error message")
    component.triggerError()
    
    verify(component.hasError, "Should be in error state")
}
```

### Data-Driven Tests

```qml
function test_multipleInputs_data() {
    return [
        { tag: "case1", input: "a", expected: "A" },
        { tag: "case2", input: "b", expected: "B" },
    ]
}

function test_multipleInputs(data) {
    var result = processInput(data.input)
    compare(result, data.expected)
}
```

## References

- [Qt Test Overview](https://doc.qt.io/qt-6/qtest-overview.html)
- [Qt Quick Test](https://doc.qt.io/qt-6/qtquicktest-index.html)
- [QML TestCase](https://doc.qt.io/qt-6/qml-qttest-testcase.html)
- [Google Test Primer](https://google.github.io/googletest/primer.html)
- [GMock for Dummies](https://google.github.io/googletest/gmock_for_dummies.html)
