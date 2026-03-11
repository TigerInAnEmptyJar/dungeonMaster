#pragma once

#include <boost/uuid/uuid.hpp>

#include <QObject>
#include <QStringList>

#include <iosfwd>
#include <memory>
#include <string>
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
 * how to detect its own format, what file-dialog filter strings it provides,
 * and how to read/write domain objects from/to a byte stream.
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

  /**
   * \brief Returns \c true if this serializer can parse a stream whose
   *        first line is \p firstLine.
   *
   * Used by \c Io::read() for automatic format detection.
   */
  virtual auto canRead(std::string const& firstLine) const -> bool = 0;

  // ── IO ────────────────────────────────────────────────────────────────────

  /**
   * \brief Reads all objects from \p stream.
   *
   * The stream contains a flat, self-describing collection of objects in the
   * serializer's own format.  \p firstLine has already been consumed by
   * \c Io::read() for format detection and is passed verbatim so the
   * serializer can reconstruct the full document.
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
  virtual auto read(std::string const& firstLine, std::istream& stream,
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
