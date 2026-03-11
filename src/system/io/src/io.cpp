#include <io.hpp>

#include <objectFactory.hpp>
#include <objectRegistry.hpp>

#include <boost/uuid/uuid.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <istream>
#include <stdexcept>
#include <string>

namespace gurps_system {

// ── Impl ──────────────────────────────────────────────────────────────────────

struct Io::Impl
{
  infrastructure::ObjectFactory& factory;
  infrastructure::ObjectRegistry& registry;
  std::unordered_map<boost::uuids::uuid, std::shared_ptr<Serializer>> serializers;

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
  if (!serializer) {
    return false;
  }
  auto const id = serializer->id();
  if (auto it = _p->serializers.find(id); it != _p->serializers.end()) {
    return false;
  }
  _p->serializers.emplace(id, std::move(serializer));
  return true;
}

auto Io::uninstall(boost::uuids::uuid const& id) -> bool
{
  if (auto it = _p->serializers.find(id); it != _p->serializers.end()) {
    _p->serializers.erase(it);
    return true;
  }
  return false;
}

// ── Serializer lookup ─────────────────────────────────────────────────────────

auto Io::installedSerializers() const -> std::vector<boost::uuids::uuid>
{
  std::vector<boost::uuids::uuid> ids;
  ids.reserve(_p->serializers.size());
  std::ranges::copy(_p->serializers | std::views::keys, std::back_inserter(ids));
  return ids;
}

auto Io::serializer(boost::uuids::uuid const& id) const -> Serializer*
{
  if (auto it = _p->serializers.find(id); it != _p->serializers.end()) {
    return it->second.get();
  }
  return nullptr;
}

auto Io::serializer(QString const& filter) const -> Serializer*
{
  if (auto it = std::ranges::find_if(
          _p->serializers, [&filter](auto const& s) { return s.second->provides(filter); });
      it != _p->serializers.end()) {
    return it->second.get();
  }
  return nullptr;
}

auto Io::allFilters() const -> QStringList
{
  QStringList result;
  for (auto const& [_, s] : _p->serializers) {
    result.append(s->filters());
  }
  return result;
}

// ── Forwarding ────────────────────────────────────────────────────────────────────

auto Io::read(std::istream& stream) const -> std::vector<std::shared_ptr<infrastructure::TreeItem>>
{
  // Read the fixed-size binary header
  Serializer::FileHeader header{};
  if (!stream.read(reinterpret_cast<char*>(header.data()), Serializer::kHeaderSize)) {
    return {};
  }

  // Verify magic bytes "DMFMT"
  static constexpr char kMagic[5] = {'D', 'M', 'F', 'M', 'T'};
  if (std::memcmp(header.data(), kMagic, 5) != 0) {
    return {};
  }

  // Extract serializer UUID from bytes 5–20
  boost::uuids::uuid formatId{};
  std::memcpy(formatId.data, header.data() + 5, 16);

  // Extract format version (big-endian uint16) from bytes 21–22
  auto const version =
      static_cast<uint16_t>((static_cast<uint16_t>(std::to_integer<uint8_t>(header[21])) << 8) |
                            static_cast<uint16_t>(std::to_integer<uint8_t>(header[22])));

  // O(1) lookup by UUID
  auto* s = serializer(formatId);
  if (!s) {
    return {};
  }

  auto items = s->read(header, version, stream, _p->factory, _p->registry);

  // Register every returned item so that ItemResolver children resolve in order.
  for (auto const& item : items) {
    _p->registry.registerObject(item);
  }

  return items;
}

auto Io::write(std::ostream& stream,
               std::vector<std::shared_ptr<infrastructure::TreeItem>> const& objects,
               QString const& filter) const -> void
{
  auto* s = serializer(filter);
  if (!s) {
    throw std::invalid_argument{"Io::write: no serializer provides filter '" +
                                filter.toStdString() + "'"};
  }
  s->write(stream, objects);
}

auto Io::write(std::ostream& stream,
               std::vector<std::shared_ptr<infrastructure::TreeItem>> const& objects,
               boost::uuids::uuid const& id) const -> void
{
  auto* s = serializer(id);
  if (!s) {
    throw std::invalid_argument{"Io::write: no serializer with the given id"};
  }
  s->write(stream, objects);
}

} // namespace gurps_system
