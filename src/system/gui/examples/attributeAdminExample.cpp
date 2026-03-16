#include <attribute.hpp>
#include <attributeContainer.hpp>
#include <attributeListModel.hpp>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <memory>

// Initialize Qt resources from static library
extern int qInitResources_qml();
static int _resourceInit = qInitResources_qml();

/**
 * \brief Example application demonstrating the Attribute management (admin) interface.
 *
 * Shows the AttributeManagementView with full CRUD capabilities:
 * - Create new attributes
 * - Edit existing attribute properties
 * - Delete attributes
 */
int main(int argc, char* argv[])
{
  QGuiApplication app(argc, argv);

  // Create a container for attributes
  auto container = std::make_shared<gurps_system::AttributeContainer>();

  // Create some sample attributes
  auto strength = std::make_shared<gurps_system::Attribute>();
  strength->setName("Strength (ST)");
  strength->setDescription("Physical power and ability to lift, carry, and damage.");
  container->insertChild(0, strength);

  auto dexterity = std::make_shared<gurps_system::Attribute>();
  dexterity->setName("Dexterity (DX)");
  dexterity->setDescription("Agility, coordination, and fine motor control.");
  container->insertChild(1, dexterity);

  auto intelligence = std::make_shared<gurps_system::Attribute>();
  intelligence->setName("Intelligence (IQ)");
  intelligence->setDescription("Mental ability, creativity, and reasoning power.");
  container->insertChild(2, intelligence);

  auto health = std::make_shared<gurps_system::Attribute>();
  health->setName("Health (HT)");
  health->setDescription("Physical fitness, disease resistance, and endurance.");
  container->insertChild(3, health);

  // Create the list model
  auto* listModel = new gurps_system::gui::AttributeListModel(&app);
  listModel->setContainer(container.get());

  // Set up QML engine
  QQmlApplicationEngine engine;

  // Expose the model to QML
  engine.rootContext()->setContextProperty("attributeModel", listModel);

  // Load the management QML interface
  engine.loadFromModule("QtQuick.Controls", "ApplicationWindow");

  // Alternative: load from inline QML
  engine.loadData(R"(
    import QtQuick
    import QtQuick.Controls
    import QtQuick.Layouts
    
    ApplicationWindow {
        visible: true
        width: 900
        height: 600
        title: "Attribute Admin - GURPS System Configuration"
        
        // Load the management view from resources
        Loader {
            anchors.fill: parent
            source: "qrc:/qml/systemGui/AttributeManagementView.qml"
            
            onLoaded: {
                // Set the model on the loaded item
                item.attributeModel = attributeModel
            }
        }
        
        menuBar: MenuBar {
            Menu {
                title: "File"
                MenuItem { text: "Save"; onTriggered: console.log("Save not yet implemented") }
                MenuItem { text: "Load"; onTriggered: console.log("Load not yet implemented") }
                MenuSeparator {}
                MenuItem { text: "Quit"; onTriggered: Qt.quit() }
            }
            Menu {
                title: "Help"
                MenuItem { text: "About"; onTriggered: console.log("Attribute Admin v1.0") }
            }
        }
    }
  )");

  if (engine.rootObjects().isEmpty()) {
    return -1;
  }

  return app.exec();
}
