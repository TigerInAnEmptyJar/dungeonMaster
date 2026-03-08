#pragma once

#include <objectRegistry.hpp>
#include <treeItem.hpp>

#include <boost/uuid/uuid.hpp>

namespace infrastructure {

/**
 * \brief A placeholder TreeItem that resolves to a target object once it appears in the registry.
 *
 * Use this when an object graph needs to reference another object that may not yet have been
 * created or registered.  ItemResolver occupies a slot in the tree; when the target object is
 * registered with the ObjectRegistry, ItemResolver automatically replaces itself with the real
 * object via TreeItem::swapChild().
 *
 * The check only takes effect while the item is part of a tree (has a parent).  If the target is
 * already registered at the time ItemResolver is inserted into the tree, the swap happens
 * immediately (deferred to the next event-loop iteration so the insertion signals have been
 * fully processed before the swap signals fire).
 *
 * After a successful resolution the ItemResolver is no longer owned by the tree and will be
 * destroyed unless the caller holds an additional shared_ptr.
 */
class ItemResolver : public TreeItem
{
  Q_OBJECT

public:
  /**
   * \brief Constructs an ItemResolver waiting for \p targetId.
   *
   * \param targetId  Instance UUID of the object this resolver is looking for.
   * \param registry  The registry to watch for the target object.
   * \param objectId  Optional explicit instance UUID for this resolver itself.
   */
  explicit ItemResolver(boost::uuids::uuid targetId, ObjectRegistry& registry,
                        boost::uuids::uuid objectId = boost::uuids::uuid{});

  ~ItemResolver() override;

  ItemResolver(ItemResolver const&) = delete;
  auto operator=(ItemResolver const&) -> ItemResolver& = delete;
  ItemResolver(ItemResolver&&) = delete;
  auto operator=(ItemResolver&&) -> ItemResolver& = delete;

  // ── Identity ──────────────────────────────────────────────────────────────

  static auto classId() -> boost::uuids::uuid;
  auto typeId() const -> boost::uuids::uuid override;

  // ── Accessors ─────────────────────────────────────────────────────────────

  /**
   * \brief Returns the instance UUID this resolver is waiting for.
   */
  auto targetId() const -> boost::uuids::uuid;

  /**
   * \brief Returns true if the resolver has already replaced itself with the target.
   */
  auto isResolved() const -> bool;

protected:
  auto onAddedToParent(std::shared_ptr<TreeItem> parent) -> void override;
  auto onRemovedFromParent() -> void override;

private:
  struct Impl;
  std::unique_ptr<Impl> _p;
};

} // namespace infrastructure
