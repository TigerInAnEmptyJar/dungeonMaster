#pragma once

#include <treeItem.hpp>

#include <boost/uuid/uuid.hpp>

#include <functional>
#include <memory>
#include <vector>

namespace infrastructure {

/**
 * \brief A factory that creates TreeItem-derived objects by type UUID.
 *
 * Creator functions are installed with install() keyed on a type UUID (typically the
 * return value of a subclass's classId()). Calling create() invokes the matching
 * creator and forwards the desired instance UUID so the resulting object has a
 * known, stable objectId.
 *
 * The factory does not own any objects it creates.
 */
class ObjectFactory
{
public:
  /// Signature of a creator function.
  ///
  /// The factory calls it with the desired instance UUID; the function must return a
  /// fully constructed, non-null shared_ptr on success.
  using Creator = std::function<std::shared_ptr<TreeItem>(boost::uuids::uuid)>;

  ObjectFactory();
  ~ObjectFactory();

  ObjectFactory(ObjectFactory const&) = delete;
  auto operator=(ObjectFactory const&) -> ObjectFactory& = delete;
  ObjectFactory(ObjectFactory&&) = delete;
  auto operator=(ObjectFactory&&) -> ObjectFactory& = delete;

  /**
   * \brief Registers \p creator under \p typeId.
   *
   * \returns \c true on success, \c false if \p typeId is already installed.
   */
  auto install(boost::uuids::uuid const& typeId, Creator creator) -> bool;

  /**
   * \brief Removes the creator for \p typeId.
   *
   * \returns \c true on success, \c false if \p typeId was not installed.
   */
  auto uninstall(boost::uuids::uuid const& typeId) -> bool;

  /**
   * \brief Returns \c true if a creator is registered for \p typeId.
   */
  auto isInstalled(boost::uuids::uuid const& typeId) const -> bool;

  /**
   * \brief Returns the number of installed creator functions.
   */
  auto installedCount() const -> int;

  /**
   * \brief Returns all currently installed type UUIDs.
   */
  auto installedTypes() const -> std::vector<boost::uuids::uuid>;

  /**
   * \brief Creates an object of type \p typeId with the given instance \p objectId.
   *
   * \returns The new object, or \c nullptr if \p typeId is not installed.
   */
  auto create(boost::uuids::uuid const& typeId, boost::uuids::uuid const& objectId) const
      -> std::shared_ptr<TreeItem>;

  /**
   * \brief Creates an object of type \p typeId with a freshly generated instance UUID.
   *
   * \returns The new object, or \c nullptr if \p typeId is not installed.
   */
  auto create(boost::uuids::uuid const& typeId) const -> std::shared_ptr<TreeItem>;

private:
  struct Impl;
  std::unique_ptr<Impl> _p;
};

} // namespace infrastructure
