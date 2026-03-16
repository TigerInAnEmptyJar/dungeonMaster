# Create the GUI library
my_add_library(systemGui
  STATIC AUTOMOC
  INCLUDES
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DEPENDS
    PUBLIC Qt6::Core
    PUBLIC Qt6::Qml
    PUBLIC DM::System::Datamodel
    PUBLIC DM::Infrastructure
  HEADER
    include/attributeListModel.hpp
  SOURCE
    src/attributeListModel.cpp
  RESOURCES
    qml.qrc
  ALIAS DM::System::Gui
)

# Optional example application
option(BUILD_GUI_EXAMPLES "Build GUI example applications" ON)

if(BUILD_GUI_EXAMPLES)
    my_add_executable(attributeGuiExample
      AUTOMOC
      SOURCE
        examples/attributeGuiExample.cpp
      DEPENDS
        PRIVATE DM::System::Gui
        PRIVATE Qt6::Qml
        PRIVATE Qt6::Quick
    )
endif()
