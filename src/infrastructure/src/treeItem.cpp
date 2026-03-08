#include "treeItem.hpp"

#include <QMetaType>

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid.hpp>

#include <algorithm>
#include <map>
#include <vector>

namespace infrastructure {

// ── Pimpl ─────────────────────────────────────────────────────────────────────

struct TreeItem::Impl
{
  boost::uuids::uuid const _objectId;
  std::weak_ptr<TreeItem> _parent;
  std::vector<std::shared_ptr<TreeItem>> _children;
  std::map<TreeItem*, std::vector<QMetaObject::Connection>> _connections;

  explicit Impl(boost::uuids::uuid objectId)
      : _objectId(objectId.is_nil() ? boost::uuids::random_generator{}() : objectId)
  {
  }

  auto connectChildSignals(TreeItem* owner, std::shared_ptr<TreeItem> const& child) -> void;
  auto disconnectChildSignals(std::shared_ptr<TreeItem> const& child) -> void;
};

auto TreeItem::Impl::connectChildSignals(TreeItem* owner, std::shared_ptr<TreeItem> const& child)
    -> void
{
  // Forward the child's subtree signals upward so any ancestor can observe
  // structural changes anywhere in the subtree below it.
  std::vector<QMetaObject::Connection> conns;
  conns.push_back(QObject::connect(child.get(), &TreeItem::subtreeChildAboutToBeInserted, owner,
                                   &TreeItem::subtreeChildAboutToBeInserted));
  conns.push_back(QObject::connect(child.get(), &TreeItem::subtreeChildInserted, owner,
                                   &TreeItem::subtreeChildInserted));
  conns.push_back(QObject::connect(child.get(), &TreeItem::subtreeChildAboutToBeRemoved, owner,
                                   &TreeItem::subtreeChildAboutToBeRemoved));
  conns.push_back(QObject::connect(child.get(), &TreeItem::subtreeChildRemoved, owner,
                                   &TreeItem::subtreeChildRemoved));
  _connections[child.get()] = std::move(conns);
}

auto TreeItem::Impl::disconnectChildSignals(std::shared_ptr<TreeItem> const& child) -> void
{
  auto it = _connections.find(child.get());
  if (it == _connections.end()) {
    return;
  }
  for (auto& conn : it->second) {
    QObject::disconnect(conn);
  }
  _connections.erase(it);
}

// ── Lifecycle ─────────────────────────────────────────────────────────────────

TreeItem::TreeItem(boost::uuids::uuid objectId) : _p(std::make_unique<Impl>(objectId))
{
  static auto const _registered = qRegisterMetaType<boost::uuids::uuid>();
  Q_UNUSED(_registered)
}

TreeItem::~TreeItem() = default;

// ── Identity ──────────────────────────────────────────────────────────────────

auto TreeItem::classId() -> boost::uuids::uuid
{
  static boost::uuids::uuid const id =
      boost::uuids::string_generator()("a48e341c-f728-4705-b870-1dd72bf6b581");
  return id;
}

auto TreeItem::typeId() const -> boost::uuids::uuid { return TreeItem::classId(); }

auto TreeItem::objectId() const -> boost::uuids::uuid { return _p->_objectId; }

// ── Tree manipulation ─────────────────────────────────────────────────────────

auto TreeItem::insertChild(int index, std::shared_ptr<TreeItem> child) -> void
{
  auto const clampedIndex = std::min(index, static_cast<int>(_p->_children.size()));

  Q_EMIT childAboutToBeInserted(clampedIndex, child);
  Q_EMIT subtreeChildAboutToBeInserted(shared_from_this(), clampedIndex, child);

  child->_p->_parent = weak_from_this();
  _p->_children.insert(_p->_children.begin() + clampedIndex, child);
  _p->connectChildSignals(this, child);

  Q_EMIT childInserted(clampedIndex, child);
  Q_EMIT subtreeChildInserted(shared_from_this(), clampedIndex, child);

  child->onAddedToParent(shared_from_this());
}

auto TreeItem::removeChild(std::shared_ptr<TreeItem> child) -> void
{
  auto it = std::find(_p->_children.begin(), _p->_children.end(), child);
  if (it == _p->_children.end()) {
    return;
  }

  auto const index = static_cast<int>(std::distance(_p->_children.begin(), it));

  Q_EMIT childAboutToBeRemoved(index, child);
  Q_EMIT subtreeChildAboutToBeRemoved(shared_from_this(), index, child);

  _p->disconnectChildSignals(child);
  child->_p->_parent.reset();
  _p->_children.erase(it);

  Q_EMIT childRemoved(index, child);
  Q_EMIT subtreeChildRemoved(shared_from_this(), index, child);

  child->onRemovedFromParent();
}

auto TreeItem::swapChild(std::shared_ptr<TreeItem> oldChild, std::shared_ptr<TreeItem> newChild)
    -> void
{
  auto it = std::find(_p->_children.begin(), _p->_children.end(), oldChild);
  if (it == _p->_children.end()) {
    return;
  }

  auto const index = static_cast<int>(std::distance(_p->_children.begin(), it));

  // Remove old child
  Q_EMIT childAboutToBeRemoved(index, oldChild);
  Q_EMIT subtreeChildAboutToBeRemoved(shared_from_this(), index, oldChild);

  _p->disconnectChildSignals(oldChild);
  oldChild->_p->_parent.reset();
  *it = newChild;

  Q_EMIT childRemoved(index, oldChild);
  Q_EMIT subtreeChildRemoved(shared_from_this(), index, oldChild);

  oldChild->onRemovedFromParent();

  // Insert new child at same position
  Q_EMIT childAboutToBeInserted(index, newChild);
  Q_EMIT subtreeChildAboutToBeInserted(shared_from_this(), index, newChild);

  newChild->_p->_parent = weak_from_this();
  _p->connectChildSignals(this, newChild);

  Q_EMIT childInserted(index, newChild);
  Q_EMIT subtreeChildInserted(shared_from_this(), index, newChild);

  newChild->onAddedToParent(shared_from_this());
}

// ── Accessors ─────────────────────────────────────────────────────────────────

auto TreeItem::parentItem() const -> std::weak_ptr<TreeItem> { return _p->_parent; }

auto TreeItem::size() const -> int { return static_cast<int>(_p->_children.size()); }

auto TreeItem::childAt(int index) const -> std::shared_ptr<TreeItem>
{
  return _p->_children.at(static_cast<std::size_t>(index));
}

auto TreeItem::begin() const -> ChildIterator { return _p->_children.cbegin(); }

auto TreeItem::end() const -> ChildIterator { return _p->_children.cend(); }

// ── Protected hooks ───────────────────────────────────────────────────────────

auto TreeItem::onAddedToParent(std::shared_ptr<TreeItem> /*parent*/) -> void {}

auto TreeItem::onRemovedFromParent() -> void {}

} // namespace infrastructure
