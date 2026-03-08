#pragma once

#include <boost/uuid/uuid.hpp>

#include <QObject>
#include <QStringList>

#include <iosfwd>
#include <memory>
#include <string>

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
   * \brief Reads a domain object from \p stream.
   *
   * \p firstLine contains the first line already consumed by \c Io::read()
   * during format detection — the serializer may prepend it when parsing.
   * Uses \p factory to instantiate objects by type UUID and registers each
   * created object in \p registry.
   *
   * \returns The root object read, or \c nullptr on error.
   */
  virtual auto read(std::string const& firstLine, std::istream& stream,
                    infrastructure::ObjectFactory& factory,
                    infrastructure::ObjectRegistry& registry) const
      -> std::shared_ptr<infrastructure::TreeItem> = 0;

  /**
   * \brief Writes \p obj and its subtree to \p stream.
   */
  virtual auto write(std::ostream& stream, infrastructure::TreeItem const& obj) const -> void = 0;

private:
  struct Impl;
  std::unique_ptr<Impl> _p;
};

} // namespace gurps_system
