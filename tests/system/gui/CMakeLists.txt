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
  INCLUDES
    PRIVATE ${CMAKE_SOURCE_DIR}/src/system/datamodel/src
)

# QML Quick Tests
my_add_qml_test(SystemGuiQmlTests
  AUTOMOC
  HEADER
    qmlTestSetup.hpp
  SOURCE
    guiQmlTests.cpp
    qmlTestSetup.cpp
  QML_FILES
    tst_attributeListView.qml
    tst_attributeDetailView.qml
    tst_attributeDetailViewEditable.qml
  DEPENDS
    DM::System::Gui
)
