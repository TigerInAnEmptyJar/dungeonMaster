#include "objectRegistry.hpp"

#include <boost/uuid/uuid.hpp>

#include <map>

namespace infrastructure {

// ── Pimpl ─────────────────────────────────────────────────────────────────────

struct ObjectRegistry::Impl
{
  // Primary map: instance UUID → non-owning handle
  std::map<boost::uuids::uuid, std::weak_ptr<TreeItem>> _objects;

  // Reverse map: raw QObject pointer → instance UUID
  // Populated in tandem with _objects so the QObject::destroyed slot can find
  // the UUID without needing to cast to TreeItem (which is already being torn down).
  std::map<QObject*, boost::uuids::uuid> _reverseMap;

  auto remove(QObject* obj) -> void;
};

auto ObjectRegistry::Impl::remove(QObject* obj) -> void
{
  auto it = _reverseMap.find(obj);
  if (it == _reverseMap.end()) {
    return;
  }
  _objects.erase(it->second);
  _reverseMap.erase(it);
}

// ── Lifecycle ─────────────────────────────────────────────────────────────────

ObjectRegistry::ObjectRegistry(QObject* parent) : QObject(parent), _p(std::make_unique<Impl>()) {}

ObjectRegistry::~ObjectRegistry() = default;

// ── Registration ──────────────────────────────────────────────────────────────

auto ObjectRegistry::registerObject(std::shared_ptr<TreeItem> object) -> void
{
  auto const id = object->objectId();

  // Idempotent: skip if already registered under this id
  if (_p->_objects.contains(id)) {
    return;
  }

  _p->_objects[id] = object;
  _p->_reverseMap[object.get()] = id;

  Q_EMIT objectRegistered(object);

  // Auto-remove when the underlying QObject is destroyed.
  // The lambda captures this by pointer; the connection is torn down automatically
  // when either this registry or the object is destroyed first.
  QObject::connect(object.get(), &QObject::destroyed, this,
                   [this](QObject* obj) { _p->remove(obj); });
}

auto ObjectRegistry::unregisterObject(boost::uuids::uuid const& id) -> void
{
  if (auto it = _p->_objects.find(id); it != _p->_objects.end()) {
    // Disconnect the auto-remove slot so it doesn't fire after explicit removal
    if (auto locked = it->second.lock()) {
      QObject::disconnect(locked.get(), &QObject::destroyed, this, nullptr);
      _p->_reverseMap.erase(locked.get());
    }

    _p->_objects.erase(it);
  }
}

// ── Query ─────────────────────────────────────────────────────────────────────

auto ObjectRegistry::findObject(boost::uuids::uuid const& id) const -> std::weak_ptr<TreeItem>
{
  if (auto it = _p->_objects.find(id); it != _p->_objects.end()) {
    return it->second;
  }
  return {};
}

auto ObjectRegistry::registeredCount() const -> int
{
  return static_cast<int>(_p->_objects.size());
}

} // namespace infrastructure
