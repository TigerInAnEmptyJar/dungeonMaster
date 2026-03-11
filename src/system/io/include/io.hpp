#pragma once

#include <serializer.hpp>

#include <boost/uuid/uuid.hpp>

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
 * \brief Central IO dispatcher for reading and writing GURPS domain objects.
 *
 * \c Io manages a collection of \c Serializer instances, each handling one
 * file format.  It selects the appropriate serializer by format UUID or by
 * file-dialog filter string.
 *
 * \par Typical startup:
 * \code
 *   infrastructure::ObjectFactory factory;
 *   registerSystemObjects(factory);
 *
 *   gurps_system::Io io{factory};
 *   io.install(std::make_shared<JsonSerializer>());
 * \endcode
 */
class Io : public QObject
{
  Q_OBJECT

public:
  /**
   * \param factory   The object factory used by serializers during \c read().
   * \param registry  The object registry populated by serializers during \c read().
   */
  explicit Io(infrastructure::ObjectFactory& factory, infrastructure::ObjectRegistry& registry);
  ~Io() override;

  Io(Io const&) = delete;
  auto operator=(Io const&) -> Io& = delete;
  Io(Io&&) = delete;
  auto operator=(Io&&) -> Io& = delete;

  // ── Serializer management ─────────────────────────────────────────────────

  /**
   * \brief Installs \p serializer.
   *
   * \returns \c true on success, \c false if a serializer with the same
   *          \c id() is already installed.
   */
  auto install(std::shared_ptr<Serializer> serializer) -> bool;

  /**
   * \brief Removes the serializer with the given \p id.
   *
   * \returns \c true on success, \c false if \p id was not installed.
   */
  auto uninstall(boost::uuids::uuid const& id) -> bool;

  /**
   * \brief Returns the UUIDs of all installed serializers.
   */
  auto installedSerializers() const -> std::vector<boost::uuids::uuid>;

  // ── Serializer lookup ─────────────────────────────────────────────────────

  /**
   * \brief Returns the serializer with the given \p id, or \c nullptr.
   */
  auto serializer(boost::uuids::uuid const& id) const -> Serializer*;

  /**
   * \brief Returns the first serializer that \c provides(\p filter), or \c nullptr.
   */
  auto serializer(QString const& filter) const -> Serializer*;

  /**
   * \brief Returns all filter strings across all installed serializers.
   */
  auto allFilters() const -> QStringList;

  // ── Forwarding ───────────────────────────────────────────────────────────

  /**
   * \brief Reads all objects from \p stream.
   *
   * Reads the first line of \p stream, finds the first installed serializer
   * for which \c canRead(firstLine) is \c true, and delegates to its
   * \c read() method.  The first line is passed to the serializer verbatim
   * so it can reconstruct the full document.
   *
   * \returns All objects read, registered in the \c ObjectRegistry supplied
   *          at construction.  An empty vector indicates no matching
   *          serializer or an empty/unparseable stream.
   */
  auto read(std::istream& stream) const -> std::vector<std::shared_ptr<infrastructure::TreeItem>>;

  /**
   * \brief Writes \p objects to \p stream using the serializer that provides \p filter.
   *
   * \throws std::invalid_argument if no installed serializer provides \p filter.
   */
  auto write(std::ostream& stream,
             std::vector<std::shared_ptr<infrastructure::TreeItem>> const& objects,
             QString const& filter) const -> void;

  /**
   * \brief Writes \p objects to \p stream using the serializer identified by \p id.
   *
   * \throws std::invalid_argument if no installed serializer has \p id.
   */
  auto write(std::ostream& stream,
             std::vector<std::shared_ptr<infrastructure::TreeItem>> const& objects,
             boost::uuids::uuid const& id) const -> void;

private:
  struct Impl;
  std::unique_ptr<Impl> _p;
};

} // namespace gurps_system
