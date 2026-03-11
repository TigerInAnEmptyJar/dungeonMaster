#pragma once

#include <boost/uuid/uuid.hpp>

#include <QObject>
#include <QStringList>

#include <array>
#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <memory>
#include <vector>

namespace infrastructure {
class ObjectFactory;
class ObjectRegistry;
class TreeItem;
} // namespace infrastructure

namespace gurps_system {

/**
 * \brief Abstract base for all format-specific serializers.
 *
 * A Serializer encapsulates one file format (e.g. JSON, XML).  It knows
 * what file-dialog filter strings it provides, and how to read/write domain
 * objects from/to a byte stream.
 *
 * Format detection is done by \c Io::read() by inspecting the fixed-size
 * binary header that every file starts with.  The header contains the magic
 * bytes \c "DMFMT", the serializer UUID, and a format version number.
 *
 * The factory needed to create objects during \c read() is passed per-call
 * by \c Io, keeping the Serializer itself stateless.
 *
 * Concrete subclasses must implement all pure-virtual methods and supply a
 * stable \c id() UUID that uniquely identifies the serializer type.
 */
class Serializer : public QObject
{
  Q_OBJECT

public:
  explicit Serializer();
  ~Serializer() override;
  Serializer(Serializer const&) = delete;
  auto operator=(Serializer const&) -> Serializer& = delete;
  Serializer(Serializer&&) = delete;
  auto operator=(Serializer&&) -> Serializer& = delete;

  // ── File header ───────────────────────────────────────────────────────────

  /**
   * \brief Size in bytes of the binary file header.
   *
   * Layout:
   * \code
   *   Offset  Size  Field
   *   0       5     Magic ASCII "DMFMT"
   *   5       16    Serializer UUID (raw bytes, same as id())
   *   21      2     Format version, big-endian uint16
   * \endcode
   */
  static constexpr std::size_t kHeaderSize = 23;

  /** \brief Fixed-size byte array that holds one file header. */
  using FileHeader = std::array<std::byte, kHeaderSize>;

  /**
   * \brief Builds the binary file header for the given serializer \p id and
   *        \p version.
   */
  static auto buildHeader(boost::uuids::uuid const& id, uint16_t version) -> FileHeader;

  // ── Identity ──────────────────────────────────────────────────────────────

  /**
   * \brief Stable UUID that uniquely identifies this serializer type.
   */
  virtual auto id() const -> boost::uuids::uuid = 0;

  // ── Capabilities ──────────────────────────────────────────────────────────

  /**
   * \brief File-dialog filter strings provided by this serializer
   *        (e.g. \c "GURPS JSON (*.json)").
   */
  virtual auto filters() const -> QStringList = 0;

  /**
   * \brief Returns \c true if this serializer handles \p filter.
   */
  virtual auto provides(QString const& filter) const -> bool = 0;

  // ── IO ────────────────────────────────────────────────────────────────────

  /**
   * \brief Reads all objects from \p stream.
   *
   * \p header and \p version are the values extracted from the 23-byte
   * binary header already consumed by \c Io::read().  Serializers may use
   * them to select schema variants or silently migrate old data.
   *
   * Child relationships encoded as UUID references are wired by inserting an
   * \c ItemResolver from \p registry for forward references, or directly via
   * \c insertChild for back references.
   *
   * Every created object is registered in \p registry.
   *
   * \returns All top-level objects in the order they appear in the stream.
   *          An empty vector indicates a parse error or empty input.
   */
  virtual auto read(FileHeader const& header, uint16_t version, std::istream& stream,
                    infrastructure::ObjectFactory& factory,
                    infrastructure::ObjectRegistry& registry) const
      -> std::vector<std::shared_ptr<infrastructure::TreeItem>> = 0;

  /**
   * \brief Writes \p objects and all objects reachable from them to \p stream.
   *
   * The serializer collects the full object graph reachable from \p objects
   * (DFS, deduplicated by objectId) and emits a flat representation where
   * child relationships are encoded as UUID references.  Shared objects
   * therefore appear exactly once in the output.
   */
  virtual auto write(std::ostream& stream,
                     std::vector<std::shared_ptr<infrastructure::TreeItem>> const& objects) const
      -> void = 0;

private:
  struct Impl;
  std::unique_ptr<Impl> _p;
};

} // namespace gurps_system
