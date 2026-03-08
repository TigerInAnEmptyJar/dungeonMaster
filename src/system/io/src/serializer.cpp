#include <serializer.hpp>

namespace gurps_system {

// ── Impl ──────────────────────────────────────────────────────────────────────

struct Serializer::Impl
{
  // reserved for future common state
};

// ── Construction ──────────────────────────────────────────────────────────────

Serializer::Serializer() : QObject{}, _p{std::make_unique<Impl>()} {}

Serializer::~Serializer() = default;

} // namespace gurps_system
