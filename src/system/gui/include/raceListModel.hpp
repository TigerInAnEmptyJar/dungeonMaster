#pragma once

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <memory>

namespace infrastructure {
class TreeItem;
class ObjectFactory;
} // namespace infrastructure

namespace gurps_system {
class Race;
}

namespace gurps_system::gui {

/**
 * \brief QAbstractListModel that exposes a list of Race objects to QML.
 *
 * This model wraps a TreeItem container (typically a RaceContainer)
 * and exposes its children as a flat list. Each row represents one Race.
 *
 * The model provides three custom roles:
 *  - NameRole: the race's name (QString)
 *  - DescriptionRole: the race's description (QString)
 *  - RaceObjectRole: the Race* object itself
 *
 * Individual race properties can be edited directly via the Race object's
 * Qt properties (e.g., race.name = "new name" in QML). The model automatically
 * updates when races change by listening to their signals.
 *
 * The model provides methods for adding and removing races from the container.
 */
class RaceListModel : public QAbstractListModel
{
  Q_OBJECT

public:
  enum RaceRoles
  {
    NameRole = Qt::UserRole + 1,
    DescriptionRole,
    RaceObjectRole // The Race* object itself
  };

  /**
   * \brief Constructs a RaceListModel with an optional parent.
   *
   * \param factory  ObjectFactory for creating race objects. Must outlive this model.
   * \param parent   Optional QObject parent.
   *
   * Call setContainer() to attach a TreeItem containing Races.
   */
  explicit RaceListModel(infrastructure::ObjectFactory* factory, QObject* parent = nullptr);
  ~RaceListModel() override;

  // ── QAbstractItemModel interface ──────────────────────────────────────────

  int rowCount(QModelIndex const& parent = QModelIndex{}) const override;
  QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const override;
  QHash<int, QByteArray> roleNames() const override;

  // ── Container management ──────────────────────────────────────────────────

  /**
   * \brief Sets the container whose children will be exposed as the model's rows.
   *
   * The model does not take ownership; the caller must ensure the container
   * outlives this model or call setContainer(nullptr) before destroying it.
   *
   * \param container  A TreeItem whose children are Race objects.
   */
  Q_INVOKABLE void setContainer(infrastructure::TreeItem* container);

  /**
   * \brief Returns a pointer to the currently attached container.
   */
  auto container() const -> infrastructure::TreeItem*;

  // ── Race management ───────────────────────────────────────────────────────

  /**
   * \brief Returns the Race object at the given row index.
   */
  Q_INVOKABLE gurps_system::Race* raceAt(int row) const;

  /**
   * \brief Adds a new race to the container.
   * \param name The name of the new race
   * \param description The description of the new race
   * \return The index of the newly added race, or -1 on failure
   */
  Q_INVOKABLE int addRace(QString const& name, QString const& description);

  /**
   * \brief Removes the race at the given row index.
   * \param row The row index of the race to remove
   * \return true if successful, false otherwise
   */
  Q_INVOKABLE bool removeRace(int row);

private:
  struct Impl;
  std::unique_ptr<Impl> _p;
};

} // namespace gurps_system::gui
