#include "itemResolver.hpp"

#include <boost/uuid/string_generator.hpp>

namespace infrastructure {

// ── Pimpl ─────────────────────────────────────────────────────────────────────

struct ItemResolver::Impl
{
  boost::uuids::uuid _targetId;
  ObjectRegistry& _registry;
  bool _resolved{false};
  QMetaObject::Connection _registryConnection;

  explicit Impl(boost::uuids::uuid targetId, ObjectRegistry& registry)
      : _targetId(targetId), _registry(registry)
  {
  }

  auto onObjectRegistered(ItemResolver* owner, std::shared_ptr<TreeItem> const& object) -> void;
  auto tryResolve(ItemResolver* owner) -> void;
};

auto ItemResolver::Impl::onObjectRegistered(ItemResolver* owner,
                                            std::shared_ptr<TreeItem> const& object) -> void
{
  if (object->objectId() == _targetId) {
    tryResolve(owner);
  }
}

auto ItemResolver::Impl::tryResolve(ItemResolver* owner) -> void
{
  if (_resolved) {
    return;
  }

  auto parent = owner->parentItem().lock();
  if (!parent) {
    return; // not in a tree yet – will retry in onAddedToParent
  }

  auto target = _registry.findObject(_targetId).lock();
  if (!target) {
    return; // target not registered yet – will retry via objectRegistered signal
  }

  _resolved = true;

  // Disconnect before the swap so we don't receive our own removal as a stale call
  QObject::disconnect(_registryConnection);

  parent->swapChild(owner->shared_from_this(), target);
}

// ── Lifecycle ─────────────────────────────────────────────────────────────────

ItemResolver::ItemResolver(boost::uuids::uuid targetId, ObjectRegistry& registry,
                           boost::uuids::uuid objectId)
    : TreeItem(objectId), _p(std::make_unique<Impl>(targetId, registry))
{
  _p->_registryConnection = QObject::connect(
      &registry, &ObjectRegistry::objectRegistered, this,
      [this](std::shared_ptr<TreeItem> object) { _p->onObjectRegistered(this, object); },
      Qt::QueuedConnection);
}

ItemResolver::~ItemResolver() = default;

// ── Identity ──────────────────────────────────────────────────────────────────

auto ItemResolver::classId() -> boost::uuids::uuid
{
  static boost::uuids::uuid const id =
      boost::uuids::string_generator()("c3d4e5f6-a7b8-4901-bcde-f01234567890");
  return id;
}

auto ItemResolver::typeId() const -> boost::uuids::uuid { return ItemResolver::classId(); }

// ── Accessors ─────────────────────────────────────────────────────────────────

auto ItemResolver::targetId() const -> boost::uuids::uuid { return _p->_targetId; }

auto ItemResolver::isResolved() const -> bool { return _p->_resolved; }

// ── Protected hooks ───────────────────────────────────────────────────────────

auto ItemResolver::onAddedToParent(std::shared_ptr<TreeItem> /*parent*/) -> void
{
  // The target may have been registered before we entered the tree – check now.
  // Defer via invokeMethod so that the insertion signals have finished propagating
  // before the swap signals fire.
  QMetaObject::invokeMethod(this, [this]() { _p->tryResolve(this); }, Qt::QueuedConnection);
}

auto ItemResolver::onRemovedFromParent() -> void
{
  // Pause watching while not in a tree – tryResolve guards on parentItem() anyway,
  // but disconnecting avoids queued calls arriving after removal.
  QObject::disconnect(_p->_registryConnection);
  _p->_registryConnection = QObject::connect(
      &_p->_registry, &ObjectRegistry::objectRegistered, this,
      [this](std::shared_ptr<TreeItem> object) { _p->onObjectRegistered(this, object); },
      Qt::QueuedConnection);
}

} // namespace infrastructure
