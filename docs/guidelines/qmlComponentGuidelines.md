# QML Component Guidelines

This document describes best practices for creating QML-based GUI components in this project.

## Architecture Overview

QML components in this project follow the Model-View pattern, bridging C++ datamodels with declarative QML user interfaces:

```
┌─────────────────┐
│   Data Layer    │  (C++ TreeItem hierarchy)
│   (TreeItem)    │
└────────┬────────┘
         │
         │ observes changes
         ▼
┌─────────────────┐
│   Model Layer   │  (QAbstractItemModel subclasses)
│(QAbstractListModel/│
│ QAbstractItemModel)│
└────────┬────────┘
         │
         │ exposes via roles
         ▼
┌─────────────────┐
│   View Layer    │  (QML components)
│     (.qml)      │
└─────────────────┘
```

## Creating Model Classes

### QAbstractListModel for Simple Lists

For flat lists of items, subclass `QAbstractListModel`:

```cpp
class MyListModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum MyRoles {
        NameRole = Qt::UserRole + 1,
        DescriptionRole,
        ObjectRole
    };
    Q_ENUM(MyRoles)
    
    explicit MyListModel(QObject* parent = nullptr);
    
    // Required overrides
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    
    // Custom methods
    Q_INVOKABLE void setContainer(TreeItem* container);
    Q_INVOKABLE MyObject* itemAt(int row) const;
    
private:
    TreeItem* m_container = nullptr;
};
```

**Key points:**
- Define custom roles starting from `Qt::UserRole + 1`
- Use `Q_ENUM` to expose roles to QML
- Mark custom methods with `Q_INVOKABLE`
- Return role names as lowercase strings in `roleNames()`

### Dynamic Updates

Connect to TreeItem signals to update the model automatically:

```cpp
void MyListModel::setContainer(TreeItem* container) {
    beginResetModel();
    
    // Disconnect old container
    if (m_container) {
        disconnect(m_container, nullptr, this, nullptr);
    }
    
    m_container = container;
    
    // Connect new container signals
    if (m_container) {
        connect(m_container, &TreeItem::childAboutToBeInserted,
                this, [this](int index) { beginInsertRows(QModelIndex(), index, index); });
        connect(m_container, &TreeItem::childInserted,
                this, &MyListModel::endInsertRows);
        connect(m_container, &TreeItem::childAboutToBeRemoved,
                this, [this](int index) { beginRemoveRows(QModelIndex(), index, index); });
        connect(m_container, &TreeItem::childRemoved,
                this, &MyListModel::endRemoveRows);
    }
    
    endResetModel();
}
```

## QML Component Structure

### List View Components

Create reusable list view components that consume models:

```qml
// MyListView.qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    
    // Public properties
    property var model: null
    
    // Public signals
    signal itemSelected(var item)
    
    // Set reasonable implicit size
    implicitWidth: 300
    implicitHeight: 400
    
    ListView {
        id: listView
        anchors.fill: parent
        model: root.model
        clip: true
        
        delegate: ItemDelegate {
            width: listView.width
            text: model.name
            
            onClicked: {
                listView.currentIndex = index
                root.itemSelected(model.objectRole)
            }
        }
        
        ScrollBar.vertical: ScrollBar {}
    }
}
```

### Detail View Components

Create detail views that display a single object's properties:

```qml
// MyDetailView.qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    
    // Public properties
    property var item: null
    
    // Set reasonable implicit size
    implicitWidth: 400
    implicitHeight: 300
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10
        
        // Display item properties
        Label {
            text: root.item ? root.item.name : ""
            font.bold: true
            font.pixelSize: 16
        }
        
        Label {
            text: root.item ? root.item.description : ""
            Layout.fillWidth: true
            wrapMode: Text.Wrap
        }
        
        // Spacer
        Item { Layout.fillHeight: true }
    }
}
```

### Composite Application Components

Combine list and detail views into complete UIs:

```qml
// MyAppMain.qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    
    property var itemModel: null
    property var selectedItem: null
    
    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal
        
        MyListView {
            SplitView.preferredWidth: 300
            SplitView.minimumWidth: 200
            model: root.itemModel
            onItemSelected: function(item) {
                root.selectedItem = item
            }
        }
        
        MyDetailView {
            SplitView.fillWidth: true
            item: root.selectedItem
        }
    }
}
```

## Qt Resource System Integration

### Creating Resource Files

QML files should be embedded using Qt's resource system for deployment:

```xml
<!-- qml.qrc -->
<RCC>
    <qresource prefix="/qml/myModule">
        <file>qml/MyListView.qml</file>
        <file>qml/MyDetailView.qml</file>
        <file>qml/MyAppMain.qml</file>
    </qresource>
</RCC>
```

### CMake Integration

Use the `my_add_library` function with `QRC_FILES`:

```cmake
my_add_library(myGuiLib
  STATIC AUTOMOC
  HEADER
    include/myListModel.hpp
  SOURCE
    src/myListModel.cpp
  QRC_FILES
    qml.qrc
  DEPENDS
    PUBLIC Qt6::Core
    PUBLIC Qt6::Qml
  ALIAS MyProject::MyGui
)
```

### Resource Initialization in Static Libraries

For static libraries, resources must be explicitly initialized before use. This is required for both applications and tests that link against static libraries containing Qt resources.

**In Applications:**

```cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>

// Declare and initialize resources from static library
extern int qInitResources_qml();  // Use your .qrc base name
static int _resourceInit = qInitResources_qml();

int main(int argc, char** argv) {
    QGuiApplication app(argc, argv);
    
    QQmlApplicationEngine engine;
    engine.load(QUrl("qrc:/qml/myModule/MyMainWindow.qml"));
    
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }
    
    return app.exec();
}
```

**In Tests:**

```cpp
#include <QtQuickTest>

// Initialize resources before test framework
extern int qInitResources_qml();
static int _resourceInit = qInitResources_qml();

QUICK_TEST_MAIN(MyQmlTests)
```

**Important Notes:**
- The function name uses the `.qrc` base name: `qInitResources_<basename>()`
- For `qml.qrc`, the function is `qInitResources_qml()`
- Initialization must happen before any QML code tries to load resources
- Using `static int` ensures initialization runs before `main()`

### Running Applications

**With Display (X11/Wayland):**
```bash
./build/bin/myGuiExample
```

**Headless (CI/Testing):**
```bash
QT_QPA_PLATFORM=offscreen ./build/test/myGuiExample
```

The `QT_QPA_PLATFORM=offscreen` environment variable tells Qt to use a virtual framebuffer instead of requiring a display server, which is essential for:
- Continuous Integration environments
- Automated testing
- Servers without graphics
- Docker containers

## Best Practices

### Component Design

1. **Single Responsibility**: Each component should have one clear purpose
2. **Reusability**: Design components to be reusable across different contexts
3. **Properties**: Expose customization through properties, not hardcoded values
4. **Signals**: Use signals for communication with parent components
5. **Implicit Size**: Always set reasonable `implicitWidth` and `implicitHeight`

### Model Design

1. **Custom Roles**: Define semantic role names, not generic ones
2. **Object Exposure**: Consider exposing the full object via a role for flexibility
3. **Dynamic Updates**: Connect to data layer signals for automatic updates
4. **Thread Safety**: All Qt model methods must run on the main thread

### Performance

1. **Lazy Loading**: Use `Loader` for complex components that aren't always visible
2. **Clip**: Enable `clip: true` on scrollable views
3. **Delegates**: Keep delegates simple; move complex logic to C++ models
4. **Caching**: Use `cacheBuffer` on ListView for smooth scrolling

### File Organization

```
src/myModule/gui/
├── CMakeLists.txt
├── qml.qrc
├── include/
│   └── myListModel.hpp
├── src/
│   └── myListModel.cpp
├── qml/
│   ├── MyListView.qml
│   ├── MyDetailView.qml
│   └── MyAppMain.qml
└── examples/
    └── myGuiExample.cpp
```

## Common Patterns

### Master-Detail View

Split view with list on left, details on right:
- Use `SplitView` for resizable panes
- Store selected item in parent component
- Pass selected item to detail view

### Editable Forms

For editable components:
- Use `TextField`, `TextArea`, `ComboBox` instead of `Label`
- Add save/cancel buttons
- Emit signals when data changes
- Consider two-way property bindings carefully

### Loading States

Handle loading and error states:
```qml
StackView {
    initialItem: model ? listComponent : loadingComponent
    
    Component {
        id: loadingComponent
        BusyIndicator { anchors.centerIn: parent }
    }
}
```

## References

- [Qt Quick Controls](https://doc.qt.io/qt-6/qtquickcontrols-index.html)
- [QML Best Practices](https://doc.qt.io/qt-6/qml-codingconventions.html)
- [Model/View Programming](https://doc.qt.io/qt-6/model-view-programming.html)
