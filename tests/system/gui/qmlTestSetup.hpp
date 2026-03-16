#pragma once

#include <attributeContainer.hpp>
#include <attributeListModel.hpp>
#include <objectFactory.hpp>
#include <registration.hpp>
#include <treeItem.hpp>

#include <QObject>
#include <QQmlContext>
#include <QQmlEngine>
#include <memory>

// Global objects for QML tests (initialized in setupQmlEngine)
inline std::unique_ptr<infrastructure::ObjectFactory> g_factory;
inline std::shared_ptr<gurps_system::AttributeContainer> g_container;
inline std::unique_ptr<gurps_system::gui::AttributeListModel> g_model;

// Custom QML engine setup
class QmlTestSetup : public QObject
{
  Q_OBJECT

public slots:
  void qmlEngineAvailable(QQmlEngine* engine)
  {
    // Register TreeItem* as a meta-type so QML can use it in method parameters
    qRegisterMetaType<infrastructure::TreeItem*>("infrastructure::TreeItem*");

    // Initialize factory and model once
    if (!g_factory) {
      g_factory = std::make_unique<infrastructure::ObjectFactory>();
      gurps_system::registerSystemObjects(*g_factory);

      g_container = std::make_shared<gurps_system::AttributeContainer>();
      g_model = std::make_unique<gurps_system::gui::AttributeListModel>(g_factory.get());
      g_model->setContainer(g_container.get());
    }

    // Expose the model to QML tests
    engine->rootContext()->setContextProperty("attributeModel", g_model.get());
  }
};
