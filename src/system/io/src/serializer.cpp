#include <serializer.hpp>

#include <cstdint>
#include <cstring>

namespace gurps_system {

// ── Impl ──────────────────────────────────────────────────────────────────────

struct Serializer::Impl
{
  // reserved for future common state
};

// ── Construction ──────────────────────────────────────────────────────────────

Serializer::Serializer() : QObject{}, _p{std::make_unique<Impl>()} {}

Serializer::~Serializer() = default;

// ── Header helpers ────────────────────────────────────────────────────────────

auto Serializer::buildHeader(boost::uuids::uuid const& id, uint16_t version) -> FileHeader
{
  FileHeader h{};
  // Magic bytes "DMFMT" at offset 0
  h[0] = std::byte{'D'};
  h[1] = std::byte{'M'};
  h[2] = std::byte{'F'};
  h[3] = std::byte{'M'};
  h[4] = std::byte{'T'};
  // Serializer UUID (16 raw bytes) at offset 5
  std::memcpy(h.data() + 5, id.data, 16);
  // Format version, big-endian uint16, at offset 21
  h[21] = std::byte{static_cast<uint8_t>(version >> 8)};
  h[22] = std::byte{static_cast<uint8_t>(version & 0xFFu)};
  return h;
}

} // namespace gurps_system
