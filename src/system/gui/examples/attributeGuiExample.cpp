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
 * \brief Example application demonstrating the Attribute GUI.
 *
 * Creates a sample set of GURPS attributes and displays them in a QML interface.
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

  auto hitPoints = std::make_shared<gurps_system::Attribute>();
  hitPoints->setName("Hit Points (HP)");
  hitPoints->setDescription("Amount of injury you can sustain before dying.");
  container->insertChild(4, hitPoints);

  auto will = std::make_shared<gurps_system::Attribute>();
  will->setName("Will");
  will->setDescription("Mental strength and resistance to psychological stress.");
  container->insertChild(5, will);

  auto perception = std::make_shared<gurps_system::Attribute>();
  perception->setName("Perception (Per)");
  perception->setDescription("General alertness and ability to notice things.");
  container->insertChild(6, perception);

  // Create the list model
  auto* listModel = new gurps_system::gui::AttributeListModel(&app);
  listModel->setContainer(container.get());

  // Set up QML engine
  QQmlApplicationEngine engine;

  // Expose the model to QML
  engine.rootContext()->setContextProperty("attributeModel", listModel);

  // Load the main QML file
  engine.load(QUrl("qrc:/qml/systemGui/AttributesMain.qml"));

  if (engine.rootObjects().isEmpty()) {
    return -1;
  }

  return app.exec();
}
