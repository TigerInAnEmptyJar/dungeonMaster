#include <parentRef.hpp>

#include <objectRegistry.hpp>

#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace gurps_system {

ParentRef::ParentRef(boost::uuids::uuid objectId) : infrastructure::TreeItem{objectId} {}

ParentRef::~ParentRef() = default;

// ── Identity ──────────────────────────────────────────────────────────────────

auto ParentRef::classId() -> boost::uuids::uuid
{
  static auto const id = boost::uuids::string_generator{}("e4f5a6b7-c8d9-4e0f-1a2b-3c4d5e6f7a8b");
  return id;
}

auto ParentRef::typeId() const -> boost::uuids::uuid { return classId(); }

// ── Target ────────────────────────────────────────────────────────────────────

auto ParentRef::targetId() const -> boost::uuids::uuid { return _targetId; }

auto ParentRef::targetIdString() const -> QString
{
  return QString::fromStdString(boost::uuids::to_string(_targetId));
}

auto ParentRef::setTargetIdString(QString const& s) -> void
{
  _targetId = boost::uuids::string_generator{}(s.toStdString());
  _resolved.reset(); // invalidate cached pointer when target changes
}

auto ParentRef::resolve(infrastructure::ObjectRegistry const& registry) -> void
{
  if (_resolved) {
    return;
  }
  auto const weak = registry.findObject(_targetId);
  if (auto item = weak.lock()) {
    _resolved = std::dynamic_pointer_cast<BaseObject>(item);
  }
}

auto ParentRef::target() const -> std::shared_ptr<BaseObject> { return _resolved; }

// ── Factory ───────────────────────────────────────────────────────────────────

auto ParentRef::pointingAt(std::shared_ptr<BaseObject> target, boost::uuids::uuid objectId)
    -> std::shared_ptr<ParentRef>
{
  auto ref = std::make_shared<ParentRef>(objectId);
  ref->_targetId = target->objectId();
  ref->_resolved = std::move(target);
  return ref;
}

} // namespace gurps_system
