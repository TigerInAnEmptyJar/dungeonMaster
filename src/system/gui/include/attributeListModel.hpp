#pragma once

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <memory>

namespace infrastructure {
class TreeItem;
}

namespace gurps_system {
class Attribute;
}

namespace gurps_system::gui {

/**
 * \brief QAbstractListModel that exposes a list of Attribute objects to QML.
 *
 * This model wraps a TreeItem container (typically an AttributeContainer)
 * and exposes its children as a flat list. Each row represents one Attribute.
 *
 * The model provides two custom roles:
 *  - NameRole: the attribute's name (QString)
 *  - DescriptionRole: the attribute's description (QString)
 *
 * The model is read-only from the QML side.
 */
class AttributeListModel : public QAbstractListModel
{
  Q_OBJECT

public:
  enum AttributeRoles
  {
    NameRole = Qt::UserRole + 1,
    DescriptionRole,
    AttributeObjectRole // The Attribute* object itself
  };

  /**
   * \brief Constructs an AttributeListModel with an optional parent.
   *
   * Call setContainer() to attach a TreeItem containing Attributes.
   */
  explicit AttributeListModel(QObject* parent = nullptr);
  ~AttributeListModel() override;

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
   * \param container  A TreeItem whose children are Attribute objects.
   */
  Q_INVOKABLE void setContainer(infrastructure::TreeItem* container);

  /**
   * \brief Returns a pointer to the currently attached container.
   */
  auto container() const -> infrastructure::TreeItem*;

private:
  struct Impl;
  std::unique_ptr<Impl> _p;
};

} // namespace gurps_system::gui
