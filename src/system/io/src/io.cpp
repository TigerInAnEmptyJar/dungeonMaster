#include <io.hpp>

#include <objectFactory.hpp>
#include <objectRegistry.hpp>

#include <algorithm>
#include <istream>
#include <stdexcept>
#include <string>

namespace gurps_system {

// ── Impl ──────────────────────────────────────────────────────────────────────

struct Io::Impl
{
  infrastructure::ObjectFactory& factory;
  infrastructure::ObjectRegistry& registry;
  std::vector<std::shared_ptr<Serializer>> serializers;

  explicit Impl(infrastructure::ObjectFactory& f, infrastructure::ObjectRegistry& r)
      : factory{f}, registry{r}
  {
  }
};

// ── Construction ──────────────────────────────────────────────────────────────

Io::Io(infrastructure::ObjectFactory& factory, infrastructure::ObjectRegistry& registry)
    : QObject{}, _p{std::make_unique<Impl>(factory, registry)}
{
}

Io::~Io() = default;

// ── Serializer management ─────────────────────────────────────────────────────

auto Io::install(std::shared_ptr<Serializer> serializer) -> bool
{
  if (!serializer)
    return false;
  auto const id = serializer->id();
  if (std::any_of(_p->serializers.begin(), _p->serializers.end(),
                  [&id](auto const& s) { return s->id() == id; }))
    return false;
  _p->serializers.push_back(std::move(serializer));
  return true;
}

auto Io::uninstall(boost::uuids::uuid const& id) -> bool
{
  auto it = std::find_if(_p->serializers.begin(), _p->serializers.end(),
                         [&id](auto const& s) { return s->id() == id; });
  if (it == _p->serializers.end())
    return false;
  _p->serializers.erase(it);
  return true;
}

// ── Serializer lookup ─────────────────────────────────────────────────────────

auto Io::installedSerializers() const -> std::vector<boost::uuids::uuid>
{
  std::vector<boost::uuids::uuid> ids;
  ids.reserve(_p->serializers.size());
  for (auto const& s : _p->serializers)
    ids.push_back(s->id());
  return ids;
}

auto Io::serializer(boost::uuids::uuid const& id) const -> Serializer*
{
  auto it = std::find_if(_p->serializers.begin(), _p->serializers.end(),
                         [&id](auto const& s) { return s->id() == id; });
  return (it != _p->serializers.end()) ? it->get() : nullptr;
}

auto Io::serializer(QString const& filter) const -> Serializer*
{
  auto it = std::find_if(_p->serializers.begin(), _p->serializers.end(),
                         [&filter](auto const& s) { return s->provides(filter); });
  return (it != _p->serializers.end()) ? it->get() : nullptr;
}

auto Io::allFilters() const -> QStringList
{
  QStringList result;
  for (auto const& s : _p->serializers)
    result << s->filters();
  return result;
}

// ── Forwarding ────────────────────────────────────────────────────────────────────

auto Io::read(std::istream& stream) const -> std::shared_ptr<infrastructure::TreeItem>
{
  std::string firstLine;
  if (!std::getline(stream, firstLine))
    return nullptr;

  auto it = std::find_if(_p->serializers.begin(), _p->serializers.end(),
                         [&firstLine](auto const& s) { return s->canRead(firstLine); });
  if (it == _p->serializers.end())
    return nullptr;

  return (*it)->read(firstLine, stream, _p->factory, _p->registry);
}

auto Io::write(std::ostream& stream, infrastructure::TreeItem const& obj,
               QString const& filter) const -> void
{
  auto* s = serializer(filter);
  if (!s)
    throw std::invalid_argument{"Io::write: no serializer provides filter '" +
                                filter.toStdString() + "'"};
  s->write(stream, obj);
}

auto Io::write(std::ostream& stream, infrastructure::TreeItem const& obj,
               boost::uuids::uuid const& id) const -> void
{
  auto* s = serializer(id);
  if (!s)
    throw std::invalid_argument{"Io::write: no serializer with the given id"};
  s->write(stream, obj);
}

} // namespace gurps_system
