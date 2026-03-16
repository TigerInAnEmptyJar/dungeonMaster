#include <professionListModel.hpp>

#include <baseObject.hpp>
#include <objectFactory.hpp>
#include <profession.hpp>
#include <treeItem.hpp>

#include <QDebug>
#include <QMetaObject>
#include <QVariant>

// Register Profession* as a metatype so it can be used with QVariant
Q_DECLARE_METATYPE(gurps_system::Profession*)

namespace gurps_system::gui {

// ── Private implementation ────────────────────────────────────────────────────

struct ProfessionListModel::Impl
{
  infrastructure::TreeItem* container{nullptr};
  infrastructure::ObjectFactory* factory{nullptr};
};

// ── Construction ──────────────────────────────────────────────────────────────

ProfessionListModel::ProfessionListModel(infrastructure::ObjectFactory* factory, QObject* parent)
    : QAbstractListModel{parent}, _p{std::make_unique<Impl>()}
{
  _p->factory = factory;
}

ProfessionListModel::~ProfessionListModel() = default;

// ── QAbstractItemModel interface ──────────────────────────────────────────────

int ProfessionListModel::rowCount(QModelIndex const& parent) const
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

QVariant ProfessionListModel::data(QModelIndex const& index, int role) const
{
  if (!index.isValid() || !_p->container) {
    return {};
  }

  int const row = index.row();
  if (row < 0 || row >= _p->container->size()) {
    return {};
  }

  auto child = _p->container->childAt(row);
  auto* profession = dynamic_cast<gurps_system::Profession*>(child.get());

  if (!profession) {
    qWarning() << "ProfessionListModel: child at row" << row << "is not a Profession";
    return {};
  }

  switch (role) {
  case NameRole:
    return profession->name();
  case DescriptionRole:
    return profession->description();
  case ProfessionObjectRole:
    return QVariant::fromValue(profession);
  default:
    return {};
  }
}

QHash<int, QByteArray> ProfessionListModel::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[NameRole] = "name";
  roles[DescriptionRole] = "description";
  roles[ProfessionObjectRole] = "professionObject";
  return roles;
}

// ── Container management ──────────────────────────────────────────────────────

void ProfessionListModel::setContainer(infrastructure::TreeItem* container)
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

auto ProfessionListModel::container() const -> infrastructure::TreeItem* { return _p->container; }

// ── Profession management ─────────────────────────────────────────────────────

gurps_system::Profession* ProfessionListModel::professionAt(int row) const
{
  if (!_p->container || row < 0 || row >= _p->container->size()) {
    return nullptr;
  }

  auto child = _p->container->childAt(row);
  return dynamic_cast<gurps_system::Profession*>(child.get());
}

int ProfessionListModel::addProfession(QString const& name, QString const& description)
{
  if (!_p->container) {
    qWarning() << "ProfessionListModel::addProfession: Cannot add profession (no container set)";
    return -1;
  }

  // Create new profession
  auto newProfession = std::make_shared<gurps_system::Profession>();
  newProfession->setName(name);
  newProfession->setDescription(description);

  // Insert at the end
  int newIndex = _p->container->size();
  _p->container->insertChild(newIndex, newProfession);

  // Connect signals for the new profession
  if (auto* baseObj = dynamic_cast<gurps_system::BaseObject*>(newProfession.get())) {
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

bool ProfessionListModel::removeProfession(int row)
{
  if (!_p->container || row < 0 || row >= _p->container->size()) {
    qWarning() << "ProfessionListModel::removeProfession: Cannot remove profession at row" << row;
    return false;
  }

  auto child = _p->container->childAt(row);
  if (!child) {
    return false;
  }

  _p->container->removeChild(child);
  return true;
}

} // namespace gurps_system::gui
