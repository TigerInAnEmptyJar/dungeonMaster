#include <attributeListModel.hpp>

#include <attribute.hpp>
#include <baseObject.hpp>
#include <objectFactory.hpp>
#include <registration.hpp>
#include <treeItem.hpp>

#include <QDebug>
#include <QMetaObject>
#include <QVariant>

// Register Attribute* as a metatype so it can be used with QVariant
Q_DECLARE_METATYPE(gurps_system::Attribute*)

namespace gurps_system::gui {

// ── Private implementation ────────────────────────────────────────────────────

struct AttributeListModel::Impl
{
  infrastructure::TreeItem* container{nullptr};
  infrastructure::ObjectFactory* factory{nullptr};
};

// ── Construction ──────────────────────────────────────────────────────────────

AttributeListModel::AttributeListModel(infrastructure::ObjectFactory* factory, QObject* parent)
    : QAbstractListModel{parent}, _p{std::make_unique<Impl>()}
{
  _p->factory = factory;
}

AttributeListModel::~AttributeListModel() = default;

// ── QAbstractItemModel interface ──────────────────────────────────────────────

int AttributeListModel::rowCount(QModelIndex const& parent) const
{
  // List models should return 0 for any parent other than the root
  if (parent.isValid()) {
    return 0;
  }

  if (!_p->container) {
    return 0;
  }

  return _p->container->size();
}

QVariant AttributeListModel::data(QModelIndex const& index, int role) const
{
  if (!index.isValid() || !_p->container) {
    return {};
  }

  int const row = index.row();
  if (row < 0 || row >= _p->container->size()) {
    return {};
  }

  auto child = _p->container->childAt(row);
  auto* attr = dynamic_cast<gurps_system::Attribute*>(child.get());

  if (!attr) {
    qWarning() << "AttributeListModel: child at row" << row << "is not an Attribute";
    return {};
  }

  switch (role) {
  case NameRole:
    return attr->name();
  case DescriptionRole:
    return attr->description();
  case AttributeObjectRole:
    return QVariant::fromValue(attr);
  default:
    return {};
  }
}

QHash<int, QByteArray> AttributeListModel::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[NameRole] = "name";
  roles[DescriptionRole] = "description";
  roles[AttributeObjectRole] = "attributeObject";
  return roles;
}

// ── Container management ──────────────────────────────────────────────────────

void AttributeListModel::setContainer(infrastructure::TreeItem* container)
{
  if (_p->container == container) {
    return;
  }

  // Disconnect old container signals
  if (_p->container) {
    disconnect(_p->container, nullptr, this, nullptr);
  }

  beginResetModel();
  _p->container = container;
  endResetModel();

  // Connect new container signals
  if (_p->container) {
    connect(_p->container, &infrastructure::TreeItem::childAboutToBeInserted, this,
            [this](int index, std::shared_ptr<infrastructure::TreeItem>) {
              beginInsertRows(QModelIndex{}, index, index);
            });

    connect(_p->container, &infrastructure::TreeItem::childInserted, this,
            [this](int, std::shared_ptr<infrastructure::TreeItem>) { endInsertRows(); });

    connect(_p->container, &infrastructure::TreeItem::childAboutToBeRemoved, this,
            [this](int index, std::shared_ptr<infrastructure::TreeItem>) {
              beginRemoveRows(QModelIndex{}, index, index);
            });

    connect(_p->container, &infrastructure::TreeItem::childRemoved, this,
            [this](int, std::shared_ptr<infrastructure::TreeItem>) { endRemoveRows(); });

    // React to changes in child properties (name, description)
    // We need to monitor all existing children for property changes
    for (int i = 0; i < _p->container->size(); ++i) {
      auto child = _p->container->childAt(i);
      if (auto* baseObj = dynamic_cast<gurps_system::BaseObject*>(child.get())) {
        connect(baseObj, &gurps_system::BaseObject::nameChanged, this, [this, i]() {
          auto idx = index(i, 0);
          Q_EMIT dataChanged(idx, idx, {NameRole});
        });

        connect(baseObj, &gurps_system::BaseObject::descriptionChanged, this, [this, i]() {
          auto idx = index(i, 0);
          Q_EMIT dataChanged(idx, idx, {DescriptionRole});
        });
      }
    }
  }
}

auto AttributeListModel::container() const -> infrastructure::TreeItem* { return _p->container; }

// ── Attribute management ──────────────────────────────────────────────────────

gurps_system::Attribute* AttributeListModel::attributeAt(int row) const
{
  if (!_p->container || row < 0 || row >= _p->container->size()) {
    return nullptr;
  }

  auto child = _p->container->childAt(row);
  return dynamic_cast<gurps_system::Attribute*>(child.get());
}

int AttributeListModel::addAttribute(QString const& name, QString const& description)
{
  if (!_p->container) {
    qWarning() << "AttributeListModel::addAttribute: Cannot add attribute (no container set)";
    return -1;
  }

  // Create new attribute
  auto newAttr = std::make_shared<gurps_system::Attribute>();
  newAttr->setName(name);
  newAttr->setDescription(description);

  // Insert at the end
  int newIndex = _p->container->size();
  _p->container->insertChild(newIndex, newAttr);

  // Connect signals for the new attribute
  if (auto* baseObj = dynamic_cast<gurps_system::BaseObject*>(newAttr.get())) {
    connect(baseObj, &gurps_system::BaseObject::nameChanged, this, [this, newIndex]() {
      auto idx = index(newIndex, 0);
      Q_EMIT dataChanged(idx, idx, {NameRole});
    });

    connect(baseObj, &gurps_system::BaseObject::descriptionChanged, this, [this, newIndex]() {
      auto idx = index(newIndex, 0);
      Q_EMIT dataChanged(idx, idx, {DescriptionRole});
    });
  }

  return newIndex;
}

bool AttributeListModel::removeAttribute(int row)
{
  if (!_p->container || row < 0 || row >= _p->container->size()) {
    qWarning() << "AttributeListModel::removeAttribute: Cannot remove attribute at row" << row;
    return false;
  }

  auto child = _p->container->childAt(row);
  if (!child) {
    return false;
  }

  _p->container->removeChild(child);
  return true;
}

// ── Formula management ────────────────────────────────────────────────────────

int AttributeListModel::getFormulaType(infrastructure::TreeItem* formula) const
{
  if (!formula) {
    return 0; // None
  }

  auto typeId = formula->typeId();

  // Check against known formula type IDs
  if (typeId == gurps_system::formulaTypeToClassId(gurps_system::FormulaType::Linear)) {
    return 1;
  }
  if (typeId == gurps_system::formulaTypeToClassId(gurps_system::FormulaType::Lookup)) {
    return 2;
  }
  if (typeId ==
      gurps_system::formulaTypeToClassId(gurps_system::FormulaType::ScaledSumDerivation)) {
    return 3;
  }
  if (typeId ==
      gurps_system::formulaTypeToClassId(gurps_system::FormulaType::QuadraticDerivation)) {
    return 4;
  }
  if (typeId == gurps_system::formulaTypeToClassId(gurps_system::FormulaType::LookupDerivation)) {
    return 5;
  }

  return 0; // Unknown/None
}

bool AttributeListModel::setAttributeFormulaType(gurps_system::Attribute* attribute,
                                                 int formulaTypeIndex)
{
  if (!attribute) {
    qWarning() << "AttributeListModel::setAttributeFormulaType: null attribute";
    return false;
  }

  if (!_p->factory) {
    qWarning() << "AttributeListModel::setAttributeFormulaType: no ObjectFactory set";
    return false;
  }

  // Handle "None" - remove formula
  if (formulaTypeIndex == 0) {
    attribute->setFormula(nullptr);
    return true;
  }

  // Map index to FormulaType enum
  auto formulaType = static_cast<gurps_system::FormulaType>(formulaTypeIndex);
  auto classId = gurps_system::formulaTypeToClassId(formulaType);

  if (classId.is_nil()) {
    qWarning() << "AttributeListModel::setAttributeFormulaType: invalid formula type"
               << formulaTypeIndex;
    return false;
  }

  // Create formula via factory
  auto newFormula = _p->factory->create(classId);
  if (!newFormula) {
    qWarning() << "AttributeListModel::setAttributeFormulaType: factory failed to create formula";
    return false;
  }

  // Set default property values using Qt's property system
  auto* formulaObj = dynamic_cast<QObject*>(newFormula.get());
  if (!formulaObj) {
    qWarning() << "AttributeListModel::setAttributeFormulaType: created object is not a QObject";
    return false;
  }

  // Attach to attribute
  attribute->setFormula(newFormula.get());
  return true;
}

} // namespace gurps_system::gui
