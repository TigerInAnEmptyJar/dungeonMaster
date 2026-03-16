# C++ unit tests for AttributeListModel
my_add_test(SystemGuiModelTests
  AUTOMOC
  DEPENDS
    DM::System::Gui
    DM::System::Datamodel
    Qt6::Core
    Qt6::Test
  SOURCE
    attributeListModelTest.cpp
)

# QML Quick Tests
my_add_qml_test(SystemGuiQmlTests
  SOURCE
    guiQmlTests.cpp
  QML_FILES
    tst_attributeListView.qml
    tst_attributeDetailView.qml
  DEPENDS
    DM::System::Gui
)
