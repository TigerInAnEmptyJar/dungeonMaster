#include <baseObject.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <QCoreApplication>

// ── Qt event loop ─────────────────────────────────────────────────────────────

namespace {
int s_argc = 0;
QCoreApplication s_app{s_argc, nullptr};

// ── Signal observer ───────────────────────────────────────────────────────────

struct MockBaseObjectObserver
{
  MOCK_METHOD(void, onNameChanged, (QString const&));
  MOCK_METHOD(void, onDescriptionChanged, (QString const&));
};

} // namespace

using namespace gurps_system;

// ── Fixture ───────────────────────────────────────────────────────────────────

class BaseObjectTest : public ::testing::Test
{
protected:
  BaseObject obj;
};

// ── Identity ──────────────────────────────────────────────────────────────────

TEST_F(BaseObjectTest, ClassIdIsNonNil) { EXPECT_FALSE(BaseObject::classId().is_nil()); }

TEST_F(BaseObjectTest, ClassIdIsStable) { EXPECT_EQ(BaseObject::classId(), BaseObject::classId()); }

TEST_F(BaseObjectTest, ClassIdDiffersFromTreeItemClassId)
{
  EXPECT_NE(BaseObject::classId(), infrastructure::TreeItem::classId());
}

TEST_F(BaseObjectTest, TypeIdMatchesClassId) { EXPECT_EQ(obj.typeId(), BaseObject::classId()); }

TEST_F(BaseObjectTest, ObjectIdIsNonNil) { EXPECT_FALSE(obj.objectId().is_nil()); }

// ── Default state ─────────────────────────────────────────────────────────────

TEST_F(BaseObjectTest, DefaultNameIsEmpty) { EXPECT_TRUE(obj.name().isEmpty()); }

TEST_F(BaseObjectTest, DefaultDescriptionIsEmpty) { EXPECT_TRUE(obj.description().isEmpty()); }

// ── name property ─────────────────────────────────────────────────────────────

TEST_F(BaseObjectTest, SetNameUpdatesName)
{
  obj.setName(QStringLiteral("Hero"));
  EXPECT_EQ(obj.name(), QStringLiteral("Hero"));
}

TEST_F(BaseObjectTest, SetNameEmitsNameChanged)
{
  MockBaseObjectObserver observer;
  QObject::connect(&obj, &BaseObject::nameChanged,
                   [&observer](QString const& v) { observer.onNameChanged(v); });

  EXPECT_CALL(observer, onNameChanged(QStringLiteral("Hero"))).Times(1);
  obj.setName(QStringLiteral("Hero"));
}

TEST_F(BaseObjectTest, SetNameToSameValueDoesNotEmitSignal)
{
  obj.setName(QStringLiteral("Hero"));

  MockBaseObjectObserver observer;
  QObject::connect(&obj, &BaseObject::nameChanged,
                   [&observer](QString const& v) { observer.onNameChanged(v); });

  EXPECT_CALL(observer, onNameChanged(::testing::_)).Times(0);
  obj.setName(QStringLiteral("Hero")); // same value
}

TEST_F(BaseObjectTest, SetNameToEmptyStringUpdatesName)
{
  obj.setName(QStringLiteral("Hero"));
  obj.setName(QString{});
  EXPECT_TRUE(obj.name().isEmpty());
}

// ── description property ──────────────────────────────────────────────────────

TEST_F(BaseObjectTest, SetDescriptionUpdatesDescription)
{
  obj.setDescription(QStringLiteral("A brave warrior."));
  EXPECT_EQ(obj.description(), QStringLiteral("A brave warrior."));
}

TEST_F(BaseObjectTest, SetDescriptionEmitsDescriptionChanged)
{
  MockBaseObjectObserver observer;
  QObject::connect(&obj, &BaseObject::descriptionChanged,
                   [&observer](QString const& v) { observer.onDescriptionChanged(v); });

  EXPECT_CALL(observer, onDescriptionChanged(QStringLiteral("A brave warrior."))).Times(1);
  obj.setDescription(QStringLiteral("A brave warrior."));
}

TEST_F(BaseObjectTest, SetDescriptionToSameValueDoesNotEmitSignal)
{
  obj.setDescription(QStringLiteral("A brave warrior."));

  MockBaseObjectObserver observer;
  QObject::connect(&obj, &BaseObject::descriptionChanged,
                   [&observer](QString const& v) { observer.onDescriptionChanged(v); });

  EXPECT_CALL(observer, onDescriptionChanged(::testing::_)).Times(0);
  obj.setDescription(QStringLiteral("A brave warrior.")); // same value
}

// ── Independence of properties ────────────────────────────────────────────────

TEST_F(BaseObjectTest, SettingNameDoesNotAffectDescription)
{
  obj.setDescription(QStringLiteral("desc"));
  obj.setName(QStringLiteral("name"));
  EXPECT_EQ(obj.description(), QStringLiteral("desc"));
}

TEST_F(BaseObjectTest, SettingDescriptionDoesNotAffectName)
{
  obj.setName(QStringLiteral("name"));
  obj.setDescription(QStringLiteral("desc"));
  EXPECT_EQ(obj.name(), QStringLiteral("name"));
}

// ── TreeItem integration ──────────────────────────────────────────────────────

TEST_F(BaseObjectTest, IsATreeItem)
{
  EXPECT_NE(dynamic_cast<infrastructure::TreeItem*>(&obj), nullptr);
}

TEST_F(BaseObjectTest, CanBeInsertedAsChildInTree)
{
  auto root = std::make_shared<infrastructure::TreeItem>();
  auto child = std::make_shared<BaseObject>();
  root->insertChild(0, child);

  ASSERT_EQ(root->size(), 1);
  EXPECT_EQ(root->childAt(0), child);
}

TEST_F(BaseObjectTest, ParentIsSetAfterInsertionIntoTree)
{
  auto root = std::make_shared<infrastructure::TreeItem>();
  auto child = std::make_shared<BaseObject>();
  root->insertChild(0, child);

  EXPECT_EQ(child->parentItem().lock(), root);
}
