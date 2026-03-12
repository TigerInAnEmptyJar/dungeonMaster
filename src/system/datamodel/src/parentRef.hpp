#pragma once

#include <baseObject.hpp>
#include <treeItem.hpp>

#include <boost/uuid/uuid.hpp>

#include <QString>

#include <memory>

namespace infrastructure {
class ObjectRegistry;
} // namespace infrastructure

namespace gurps_system {

/**
 * \brief A proxy tree child that carries a strong reference to a parent-attribute definition.
 *
 * \c SecondaryAttribute owns one \c ParentRef per attribute it depends on.
 * Because \c infrastructure::TreeItem permits only one structural parent, a
 * \c ParentRef acts as an indirection: multiple secondary attributes can each
 * own a \c ParentRef pointing at the same target (e.g. IQ → Will and IQ → Perception)
 * without placing the target in two distinct tree branches simultaneously.
 *
 * The target UUID is exposed as a Qt property serialised as a UUID string.
 * After deserialisation, call \c resolve() to populate the strong pointer from an
 * object registry; until then \c target() returns \c nullptr.
 *
 * \note Unlike \c infrastructure::ItemResolver, \c ParentRef is permanent:
 *       it never removes itself from the tree after resolving.
 *
 * \note Use \c ParentRef::pointingAt() as a convenience factory when the target
 *       object is already available in memory.
 */
class ParentRef final : public infrastructure::TreeItem
{
  Q_OBJECT

  Q_PROPERTY(QString targetId READ targetIdString WRITE setTargetIdString
                 DESIGNABLE false SCRIPTABLE false)

public:
  /**
   * \brief Default constructor; the target UUID is nil until set via \c setTargetIdString().
   *
   * \param objectId  Optional stable identity for this ref node.
   */
  explicit ParentRef(boost::uuids::uuid objectId = boost::uuids::uuid{});
  ~ParentRef() override;

  ParentRef(ParentRef const&) = delete;
  auto operator=(ParentRef const&) -> ParentRef& = delete;
  ParentRef(ParentRef&&) = delete;
  auto operator=(ParentRef&&) -> ParentRef& = delete;

  // ── Identity ──────────────────────────────────────────────────────────────

  static auto classId() -> boost::uuids::uuid;
  auto typeId() const -> boost::uuids::uuid override;

  // ── Target ────────────────────────────────────────────────────────────────

  /**
   * \brief Returns the UUID of the referenced attribute definition.
   */
  auto targetId() const -> boost::uuids::uuid;

  /**
   * \brief Returns the target UUID as a string (for Qt property serialisation).
   */
  auto targetIdString() const -> QString;

  /**
   * \brief Sets the target UUID from a string (called by the deserialiser).
   *
   * Clears the cached strong pointer so that the next \c resolve() call
   * re-populates it with the updated target.
   */
  auto setTargetIdString(QString const& s) -> void;

  /**
   * \brief Looks up the target in \p registry and caches a strong reference.
   *
   * Has no effect if the pointer is already populated or if the registry does
   * not contain an object matching the stored UUID.
   */
  auto resolve(infrastructure::ObjectRegistry const& registry) -> void;

  /**
   * \brief Returns the resolved strong reference, or \c nullptr before \c resolve().
   */
  auto target() const -> std::shared_ptr<BaseObject>;

  // ── Factory ───────────────────────────────────────────────────────────────

  /**
   * \brief Creates a \c ParentRef whose target is already populated.
   *
   * \param target    The attribute definition this ref points to.  Must be non-null.
   * \param objectId  Optional stable identity for the ref node.
   */
  static auto pointingAt(std::shared_ptr<BaseObject> target,
                         boost::uuids::uuid objectId = boost::uuids::uuid{})
      -> std::shared_ptr<ParentRef>;

private:
  boost::uuids::uuid _targetId{};
  std::shared_ptr<BaseObject> _resolved{};
};

} // namespace gurps_system
