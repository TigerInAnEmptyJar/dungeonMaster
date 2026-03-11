#pragma once
#include <treeItem.hpp>

#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid.hpp>

#include <QString>
#include <QStringList>

class AlphaItem : public infrastructure::TreeItem
{
  Q_OBJECT
  Q_PROPERTY(QString name READ name WRITE setName)
  Q_PROPERTY(int level READ level WRITE setLevel)
public:
  explicit AlphaItem(boost::uuids::uuid id = {}) : TreeItem(id) {}

  auto name() const -> QString { return _name; }
  auto setName(QString value) -> void { _name = std::move(value); }
  auto level() const -> int { return _level; }
  auto setLevel(int value) -> void { _level = value; }

  static auto classId() -> boost::uuids::uuid
  {
    static auto const id = boost::uuids::string_generator{}("aaaaaaaa-0000-4000-8000-000000000001");
    return id;
  }
  auto typeId() const -> boost::uuids::uuid override { return classId(); }

private:
  QString _name{};
  int _level{0};
};

class BetaItem : public infrastructure::TreeItem
{
  Q_OBJECT
  Q_PROPERTY(QStringList tags READ tags WRITE setTags)
public:
  explicit BetaItem(boost::uuids::uuid id = {}) : TreeItem(id) {}

  auto tags() const -> QStringList { return _tags; }
  auto setTags(QStringList value) -> void { _tags = std::move(value); }

  static auto classId() -> boost::uuids::uuid
  {
    static auto const id = boost::uuids::string_generator{}("bbbbbbbb-0000-4000-8000-000000000002");
    return id;
  }
  auto typeId() const -> boost::uuids::uuid override { return classId(); }

private:
  QStringList _tags{};
};

class GammaItem : public AlphaItem
{
  Q_OBJECT
  Q_PROPERTY(QString title READ title WRITE setTitle)
public:
  explicit GammaItem(boost::uuids::uuid id = {}) : AlphaItem(id) {}

  auto title() const -> QString { return _title; }
  auto setTitle(QString value) -> void { _title = std::move(value); }

  static auto classId() -> boost::uuids::uuid
  {
    static auto const id = boost::uuids::string_generator{}("cccccccc-0000-4000-8000-000000000003");
    return id;
  }
  auto typeId() const -> boost::uuids::uuid override { return classId(); }

private:
  QString _title{};
};

class DeltaItem : public infrastructure::TreeItem
{
  Q_OBJECT
public:
  enum class ItemType
  {
    Alpha,
    Beta,
    Gamma,
    Delta
  };
  Q_ENUM(ItemType)
  Q_PROPERTY(ItemType score READ score WRITE setScore)
public:
  explicit DeltaItem(boost::uuids::uuid id = {}) : TreeItem(id) {}

  auto score() const -> ItemType { return _score; }
  auto setScore(ItemType value) -> void { _score = value; }

  static auto classId() -> boost::uuids::uuid
  {
    static auto const id = boost::uuids::string_generator{}("dddddddd-0000-4000-8000-000000000004");
    return id;
  }
  auto typeId() const -> boost::uuids::uuid override { return classId(); }

private:
  ItemType _score{ItemType::Alpha};
};
