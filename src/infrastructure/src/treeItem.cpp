#include "treeItem.hpp"

#include <algorithm>
#include <map>
#include <vector>

namespace infrastructure {

// ── Pimpl ─────────────────────────────────────────────────────────────────────

struct TreeItem::Impl
{
  std::weak_ptr<TreeItem> _parent;
  std::vector<std::shared_ptr<TreeItem>> _children;
  std::map<TreeItem*, std::vector<QMetaObject::Connection>> _connections;

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

TreeItem::TreeItem(QObject* parent) : QObject(parent), _p(std::make_unique<Impl>()) {}

TreeItem::~TreeItem() = default;

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

} // namespace infrastructure
