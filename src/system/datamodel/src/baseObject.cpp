#include "baseObject.hpp"

#include <boost/uuid/string_generator.hpp>

namespace gurps_system {

// ── Pimpl ─────────────────────────────────────────────────────────────────────

struct BaseObject::Impl
{
  QString _name;
  QString _description;
};

// ── Lifecycle ─────────────────────────────────────────────────────────────────

BaseObject::BaseObject(boost::uuids::uuid objectId)
    : infrastructure::TreeItem(objectId), _p(std::make_unique<Impl>())
{
}

BaseObject::~BaseObject() = default;

// ── Identity ──────────────────────────────────────────────────────────────────

auto BaseObject::classId() -> boost::uuids::uuid
{
  static boost::uuids::uuid const id =
      boost::uuids::string_generator()("d1e2f3a4-b5c6-4d7e-8f90-a1b2c3d4e5f6");
  return id;
}

auto BaseObject::typeId() const -> boost::uuids::uuid { return BaseObject::classId(); }

// ── Properties ────────────────────────────────────────────────────────────────

auto BaseObject::name() const -> QString { return _p->_name; }

auto BaseObject::setName(QString const& name) -> void
{
  if (_p->_name == name) {
    return;
  }
  _p->_name = name;
  Q_EMIT nameChanged(_p->_name);
}

auto BaseObject::description() const -> QString { return _p->_description; }

auto BaseObject::setDescription(QString const& description) -> void
{
  if (_p->_description == description) {
    return;
  }
  _p->_description = description;
  Q_EMIT descriptionChanged(_p->_description);
}

} // namespace gurps_system
