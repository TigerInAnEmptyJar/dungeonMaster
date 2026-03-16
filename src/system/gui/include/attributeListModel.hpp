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
class Attribute;
}

namespace gurps_system::gui {

/**
 * \brief QAbstractListModel that exposes a list of Attribute objects to QML.
 *
 * This model wraps a TreeItem container (typically an AttributeContainer)
 * and exposes its children as a flat list. Each row represents one Attribute.
 *
 * The model provides three custom roles:
 *  - NameRole: the attribute's name (QString)
 *  - DescriptionRole: the attribute's description (QString)
 *  - AttributeObjectRole: the Attribute* object itself
 *
 * Individual attribute properties can be edited directly via the Attribute object's
 * Qt properties (e.g., attribute.name = "new name" in QML). The model automatically
 * updates when attributes change by listening to their signals.
 *
 * The model provides methods for adding and removing attributes from the container.
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
   * \param factory  ObjectFactory for creating formula objects. Must outlive this model.
   * \param parent   Optional QObject parent.
   *
   * Call setContainer() to attach a TreeItem containing Attributes.
   */
  explicit AttributeListModel(infrastructure::ObjectFactory* factory, QObject* parent = nullptr);
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

  // ── Attribute management ──────────────────────────────────────────────────

  /**
   * \brief Returns the Attribute object at the given row index.
   */
  Q_INVOKABLE gurps_system::Attribute* attributeAt(int row) const;

  /**
   * \brief Adds a new attribute to the container.
   * \param name The name of the new attribute
   * \param description The description of the new attribute
   * \return The index of the newly added attribute, or -1 on failure
   */
  Q_INVOKABLE int addAttribute(QString const& name, QString const& description);

  /**
   * \brief Removes the attribute at the given row index.
   * \param row The row index of the attribute to remove
   * \return true if successful, false otherwise
   */
  Q_INVOKABLE bool removeAttribute(int row);

  // ── Formula management ────────────────────────────────────────────────────

  /**
   * \brief Returns the formula type index for a given formula object.
   *
   * Determines the type of formula by checking its typeId against known formula types.
   * Returns indices matching the ComboBox:
   * - 0: None/Unknown
   * - 1: LinearFormula
   * - 2: LookupFormula
   * - 3: ScaledSumDerivationFormula
   * - 4: QuadraticDerivationFormula
   * - 5: LookupDerivationFormula
   *
   * \param formula The formula object to check (can be nullptr)
   * \return The formula type index (0-5)
   */
  Q_INVOKABLE int getFormulaType(infrastructure::TreeItem* formula) const;

  /**
   * \brief Creates and sets a formula of the specified type for an attribute.
   *
   * This method creates a formula with default parameters and attaches it to
   * the attribute. The formulaTypeIndex corresponds to the ComboBox indices:
   * - 0: None (removes the formula)
   * - 1: LinearFormula (default: 10 CP/level)
   * - 2: LookupFormula (default: {0,0}, {1,10}, {2,25})
   * - 3: ScaledSumDerivationFormula (default: coefficients={1}, divisor=1)
   * - 4: QuadraticDerivationFormula (default: divisor=5)
   * - 5: LookupDerivationFormula (default: {10,10}, {12,11})
   *
   * \param attribute The attribute to modify
   * \param formulaTypeIndex The type of formula to create (0-5)
   * \return true if successful, false otherwise
   */
  Q_INVOKABLE bool setAttributeFormulaType(gurps_system::Attribute* attribute,
                                           int formulaTypeIndex);

private:
  struct Impl;
  std::unique_ptr<Impl> _p;
};

} // namespace gurps_system::gui
