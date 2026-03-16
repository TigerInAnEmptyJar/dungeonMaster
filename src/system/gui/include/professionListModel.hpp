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
class Profession;
}

namespace gurps_system::gui {

/**
 * \brief QAbstractListModel that exposes a list of Profession objects to QML.
 *
 * This model wraps a TreeItem container (typically a ProfessionContainer)
 * and exposes its children as a flat list. Each row represents one Profession.
 *
 * The model provides three custom roles:
 *  - NameRole: the profession's name (QString)
 *  - DescriptionRole: the profession's description (QString)
 *  - ProfessionObjectRole: the Profession* object itself
 *
 * Individual profession properties can be edited directly via the Profession object's
 * Qt properties (e.g., profession.name = "new name" in QML). The model automatically
 * updates when professions change by listening to their signals.
 *
 * The model provides methods for adding and removing professions from the container.
 */
class ProfessionListModel : public QAbstractListModel
{
  Q_OBJECT

public:
  enum ProfessionRoles
  {
    NameRole = Qt::UserRole + 1,
    DescriptionRole,
    ProfessionObjectRole // The Profession* object itself
  };

  /**
   * \brief Constructs a ProfessionListModel with an optional parent.
   *
   * \param factory  ObjectFactory for creating profession objects. Must outlive this model.
   * \param parent   Optional QObject parent.
   *
   * Call setContainer() to attach a TreeItem containing Professions.
   */
  explicit ProfessionListModel(infrastructure::ObjectFactory* factory, QObject* parent = nullptr);
  ~ProfessionListModel() override;

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
   * \param container  A TreeItem whose children are Profession objects.
   */
  Q_INVOKABLE void setContainer(infrastructure::TreeItem* container);

  /**
   * \brief Returns a pointer to the currently attached container.
   */
  auto container() const -> infrastructure::TreeItem*;

  // ── Profession management ─────────────────────────────────────────────────

  /**
   * \brief Returns the Profession object at the given row index.
   */
  Q_INVOKABLE gurps_system::Profession* professionAt(int row) const;

  /**
   * \brief Adds a new profession to the container.
   * \param name The name of the new profession
   * \param description The description of the new profession
   * \return The index of the newly added profession, or -1 on failure
   */
  Q_INVOKABLE int addProfession(QString const& name, QString const& description);

  /**
   * \brief Removes the profession at the given row index.
   * \param row The row index of the profession to remove
   * \return true if successful, false otherwise
   */
  Q_INVOKABLE bool removeProfession(int row);

private:
  struct Impl;
  std::unique_ptr<Impl> _p;
};

} // namespace gurps_system::gui
