#pragma once

#include <treeItem.hpp>

#include <boost/uuid/uuid.hpp>

#include <QObject>

#include <memory>

namespace infrastructure {

/**
 * \brief A non-owning registry that maps instance UUIDs to TreeItem objects.
 *
 * Objects are registered explicitly via registerObject(). The registry holds only a
 * std::weak_ptr so it does not extend the lifetime of any object. When a registered
 * object is destroyed the entry is removed automatically by connecting to
 * QObject::destroyed.
 *
 * findObject() returns a weak_ptr; callers must lock() it before use.
 */
class ObjectRegistry : public QObject
{
  Q_OBJECT

public:
  explicit ObjectRegistry(QObject* parent = nullptr);
  ~ObjectRegistry() override;

  ObjectRegistry(ObjectRegistry const&) = delete;
  auto operator=(ObjectRegistry const&) -> ObjectRegistry& = delete;
  ObjectRegistry(ObjectRegistry&&) = delete;
  auto operator=(ObjectRegistry&&) -> ObjectRegistry& = delete;

  /**
   * \brief Registers \p object under its objectId().
   *
   * Does nothing if an object with the same objectId is already registered.
   *
   * \param object  The object to register. Must not be null.
   */
  auto registerObject(std::shared_ptr<TreeItem> object) -> void;

  /**
   * \brief Removes the entry for \p id from the registry.
   *
   * Does nothing if \p id is not registered.
   *
   * \param id  Instance UUID of the object to remove.
   */
  auto unregisterObject(boost::uuids::uuid const& id) -> void;

  /**
   * \brief Returns the registered object for \p id, or an empty weak_ptr if not found.
   *
   * \param id  Instance UUID to look up.
   * \returns   A weak_ptr to the object, or an empty weak_ptr.
   */
  auto findObject(boost::uuids::uuid const& id) const -> std::weak_ptr<TreeItem>;

  /**
   * \brief Returns the number of entries currently held in the registry.
   */
  auto registeredCount() const -> int;

Q_SIGNALS:
  /**
   * \brief Emitted after an object has been successfully registered.
   *
   * Not emitted when a duplicate registration is silently ignored.
   *
   * \param object  The newly registered object.
   */
  void objectRegistered(std::shared_ptr<TreeItem> object);

private:
  struct Impl;
  std::unique_ptr<Impl> _p;
};

} // namespace infrastructure
