#pragma once

#include <serializer.hpp>

#include <memory>
#include <vector>

namespace gurps_system {

/**
 * \brief JSON serializer for TreeItem-derived objects.
 *
 * Writes a 23-byte binary header (magic \c "DMFMT" + UUID + version) followed
 * by a flat JSON array.  Each entry represents one reachable object and contains
 * its \c typeId, \c objectId, and an optional \c children array of child
 * instance UUIDs.  The traversal is depth-first, deduplicated by objectId; a
 * shared child is only listed under the first parent that DFS-visits it.
 *
 * Wire format:
 * \code
 * [DMFMT header — 23 bytes]
 * [
 *   { "typeId": "<uuid>", "objectId": "<uuid>",
 *     "children": ["<childObjectId>", ...] },
 *   { "typeId": "<uuid>", "objectId": "<uuid>" }
 * ]
 * \endcode
 *
 * During \c read(), each entry is re-created via the supplied \c ObjectFactory.
 * Child slots are filled with \c ItemResolver placeholders that swap themselves
 * for the target object once it appears in the \c ObjectRegistry.  The event
 * loop is pumped inside \c read() before it returns so that all resolvers fire
 * while the item shared_ptrs are still alive; the tree is fully wired by the
 * time \c read() returns.
 *
 * \c read() returns only the root objects (those not referenced as children
 * by any other entry).
 */
class JsonSerializer : public Serializer
{
  Q_OBJECT

public:
  explicit JsonSerializer();
  ~JsonSerializer() override;

  JsonSerializer(JsonSerializer const&) = delete;
  auto operator=(JsonSerializer const&) -> JsonSerializer& = delete;
  JsonSerializer(JsonSerializer&&) = delete;
  auto operator=(JsonSerializer&&) -> JsonSerializer& = delete;

  // ── Identity ──────────────────────────────────────────────────────────────

  static auto classId() -> boost::uuids::uuid;
  auto id() const -> boost::uuids::uuid override;

  // ── Capabilities ──────────────────────────────────────────────────────────

  auto filters() const -> QStringList override;
  auto provides(QString const& filter) const -> bool override;

  // ── IO ────────────────────────────────────────────────────────────────────

  auto read(FileHeader const& header, uint16_t version, std::istream& stream,
            infrastructure::ObjectFactory& factory, infrastructure::ObjectRegistry& registry) const
      -> std::vector<std::shared_ptr<infrastructure::TreeItem>> override;

  auto write(std::ostream& stream,
             std::vector<std::shared_ptr<infrastructure::TreeItem>> const& objects) const
      -> void override;

private:
  struct Impl;
  std::unique_ptr<Impl> _p;
};

} // namespace gurps_system
