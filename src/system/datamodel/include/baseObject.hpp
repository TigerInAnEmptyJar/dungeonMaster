#pragma once

#include <treeItem.hpp>

#include <QObject>
#include <QString>

namespace dm {

/**
 * \brief Base class for all named, describable objects in the GURPS system datamodel.
 *
 * Extends infrastructure::TreeItem with a \c name and a \c description Qt property,
 * making both fields available to the Qt property system and QML.
 */
class BaseObject : public infrastructure::TreeItem
{
  Q_OBJECT

  Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
  Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)

public:
  explicit BaseObject(boost::uuids::uuid objectId = boost::uuids::uuid{});
  ~BaseObject() override;

  BaseObject(BaseObject const&) = delete;
  auto operator=(BaseObject const&) -> BaseObject& = delete;
  BaseObject(BaseObject&&) = delete;
  auto operator=(BaseObject&&) -> BaseObject& = delete;

  // ── Identity ──────────────────────────────────────────────────────────────

  static auto classId() -> boost::uuids::uuid;
  auto typeId() const -> boost::uuids::uuid override;

  // ── Properties ────────────────────────────────────────────────────────────

  /**
   * \brief Returns the display name of this object.
   */
  auto name() const -> QString;

  /**
   * \brief Sets the display name of this object.
   *
   * Emits \c nameChanged if the value differs from the current one.
   */
  auto setName(QString const& name) -> void;

  /**
   * \brief Returns the description of this object.
   */
  auto description() const -> QString;

  /**
   * \brief Sets the description of this object.
   *
   * Emits \c descriptionChanged if the value differs from the current one.
   */
  auto setDescription(QString const& description) -> void;

Q_SIGNALS:
  void nameChanged(QString const& name);
  void descriptionChanged(QString const& description);

private:
  struct Impl;
  std::unique_ptr<Impl> _p;
};

} // namespace dm
