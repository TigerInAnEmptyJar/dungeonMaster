#include "objectFactory.hpp"

#include <boost/uuid/random_generator.hpp>

#include <map>

namespace infrastructure {

// ── Pimpl ─────────────────────────────────────────────────────────────────────

struct ObjectFactory::Impl
{
  std::map<boost::uuids::uuid, Creator> _creators;
};

// ── Lifecycle ─────────────────────────────────────────────────────────────────

ObjectFactory::ObjectFactory() : _p(std::make_unique<Impl>()) {}

ObjectFactory::~ObjectFactory() = default;

// ── Management ────────────────────────────────────────────────────────────────

auto ObjectFactory::install(boost::uuids::uuid const& typeId, Creator creator) -> bool
{
  if (_p->_creators.contains(typeId)) {
    return false;
  }
  _p->_creators.emplace(typeId, std::move(creator));
  return true;
}

auto ObjectFactory::uninstall(boost::uuids::uuid const& typeId) -> bool
{
  return _p->_creators.erase(typeId) > 0;
}

auto ObjectFactory::isInstalled(boost::uuids::uuid const& typeId) const -> bool
{
  return _p->_creators.contains(typeId);
}

auto ObjectFactory::installedCount() const -> int { return static_cast<int>(_p->_creators.size()); }

auto ObjectFactory::installedTypes() const -> std::vector<boost::uuids::uuid>
{
  auto types = std::vector<boost::uuids::uuid>{};
  types.reserve(_p->_creators.size());
  for (auto const& [id, _] : _p->_creators) {
    types.push_back(id);
  }
  return types;
}

// ── Creation ──────────────────────────────────────────────────────────────────

auto ObjectFactory::create(boost::uuids::uuid const& typeId,
                           boost::uuids::uuid const& objectId) const -> std::shared_ptr<TreeItem>
{
  auto it = _p->_creators.find(typeId);
  if (it == _p->_creators.end()) {
    return nullptr;
  }
  return it->second(objectId);
}

auto ObjectFactory::create(boost::uuids::uuid const& typeId) const -> std::shared_ptr<TreeItem>
{
  return create(typeId, boost::uuids::random_generator{}());
}

} // namespace infrastructure
