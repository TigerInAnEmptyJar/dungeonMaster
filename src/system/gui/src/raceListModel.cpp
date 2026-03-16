#include <raceListModel.hpp>

#include <baseObject.hpp>
#include <objectFactory.hpp>
#include <race.hpp>
#include <treeItem.hpp>

#include <QDebug>
#include <QMetaObject>
#include <QVariant>

// Register Race* as a metatype so it can be used with QVariant
Q_DECLARE_METATYPE(gurps_system::Race*)

namespace gurps_system::gui {

// ── Private implementation ────────────────────────────────────────────────────

struct RaceListModel::Impl
{
  infrastructure::TreeItem* container{nullptr};
  infrastructure::ObjectFactory* factory{nullptr};
};

// ── Construction ──────────────────────────────────────────────────────────────

RaceListModel::RaceListModel(infrastructure::ObjectFactory* factory, QObject* parent)
    : QAbstractListModel{parent}, _p{std::make_unique<Impl>()}
{
  _p->factory = factory;
}

RaceListModel::~RaceListModel() = default;

// ── QAbstractItemModel interface ──────────────────────────────────────────────

int RaceListModel::rowCount(QModelIndex const& parent) const
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

QVariant RaceListModel::data(QModelIndex const& index, int role) const
{
  if (!index.isValid() || !_p->container) {
    return {};
  }

  int const row = index.row();
  if (row < 0 || row >= _p->container->size()) {
    return {};
  }

  auto child = _p->container->childAt(row);
  auto* race = dynamic_cast<gurps_system::Race*>(child.get());

  if (!race) {
    qWarning() << "RaceListModel: child at row" << row << "is not a Race";
    return {};
  }

  switch (role) {
  case NameRole:
    return race->name();
  case DescriptionRole:
    return race->description();
  case RaceObjectRole:
    return QVariant::fromValue(race);
  default:
    return {};
  }
}

QHash<int, QByteArray> RaceListModel::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[NameRole] = "name";
  roles[DescriptionRole] = "description";
  roles[RaceObjectRole] = "raceObject";
  return roles;
}

// ── Container management ──────────────────────────────────────────────────────

void RaceListModel::setContainer(infrastructure::TreeItem* container)
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

auto RaceListModel::container() const -> infrastructure::TreeItem* { return _p->container; }

// ── Race management ───────────────────────────────────────────────────────────

gurps_system::Race* RaceListModel::raceAt(int row) const
{
  if (!_p->container || row < 0 || row >= _p->container->size()) {
    return nullptr;
  }

  auto child = _p->container->childAt(row);
  return dynamic_cast<gurps_system::Race*>(child.get());
}

int RaceListModel::addRace(QString const& name, QString const& description)
{
  if (!_p->container) {
    qWarning() << "RaceListModel::addRace: Cannot add race (no container set)";
    return -1;
  }

  // Create new race
  auto newRace = std::make_shared<gurps_system::Race>();
  newRace->setName(name);
  newRace->setDescription(description);

  // Insert at the end
  int newIndex = _p->container->size();
  _p->container->insertChild(newIndex, newRace);

  // Connect signals for the new race
  if (auto* baseObj = dynamic_cast<gurps_system::BaseObject*>(newRace.get())) {
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

bool RaceListModel::removeRace(int row)
{
  if (!_p->container || row < 0 || row >= _p->container->size()) {
    qWarning() << "RaceListModel::removeRace: Cannot remove race at row" << row;
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
