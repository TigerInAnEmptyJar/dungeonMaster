#include <objectFactory.hpp>
#include <treeItem.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid.hpp>

// QCoreApplication is provided by treeItemTest.cpp (same test executable).

using namespace infrastructure;

// ── Test double ───────────────────────────────────────────────────────────────

namespace {

// A concrete TreeItem subclass with a stable, well-known classId.
class WidgetItem : public TreeItem
{
public:
  explicit WidgetItem(boost::uuids::uuid id) : TreeItem(id) {}

  static auto classId() -> boost::uuids::uuid
  {
    static boost::uuids::uuid const id =
        boost::uuids::string_generator()("11223344-5566-7788-99aa-bbccddeeff00");
    return id;
  }

  auto typeId() const -> boost::uuids::uuid override { return WidgetItem::classId(); }
};

class GadgetItem : public TreeItem
{
public:
  explicit GadgetItem(boost::uuids::uuid id) : TreeItem(id) {}

  static auto classId() -> boost::uuids::uuid
  {
    static boost::uuids::uuid const id =
        boost::uuids::string_generator()("aabbccdd-eeff-0011-2233-445566778899");
    return id;
  }

  auto typeId() const -> boost::uuids::uuid override { return GadgetItem::classId(); }
};

// Convenience creator lambdas
auto widgetCreator() -> ObjectFactory::Creator
{
  return [](boost::uuids::uuid id) -> std::shared_ptr<TreeItem> {
    return std::make_shared<WidgetItem>(id);
  };
}

auto gadgetCreator() -> ObjectFactory::Creator
{
  return [](boost::uuids::uuid id) -> std::shared_ptr<TreeItem> {
    return std::make_shared<GadgetItem>(id);
  };
}

} // namespace

// ── Fixture ───────────────────────────────────────────────────────────────────

class ObjectFactoryTest : public ::testing::Test
{
protected:
  ObjectFactory factory;
};

// ── Initial state ─────────────────────────────────────────────────────────────

TEST_F(ObjectFactoryTest, InitiallyEmpty)
{
  EXPECT_EQ(factory.installedCount(), 0);
  EXPECT_TRUE(factory.installedTypes().empty());
}

TEST_F(ObjectFactoryTest, CreateOnEmptyFactoryReturnsNullptr)
{
  EXPECT_EQ(factory.create(WidgetItem::classId()), nullptr);
}

// ── install ───────────────────────────────────────────────────────────────────

TEST_F(ObjectFactoryTest, InstallIncreasesCount)
{
  factory.install(WidgetItem::classId(), widgetCreator());
  EXPECT_EQ(factory.installedCount(), 1);
}

TEST_F(ObjectFactoryTest, InstallTwoTypesCountIsTwo)
{
  factory.install(WidgetItem::classId(), widgetCreator());
  factory.install(GadgetItem::classId(), gadgetCreator());
  EXPECT_EQ(factory.installedCount(), 2);
}

TEST_F(ObjectFactoryTest, InstallReturnsTrueOnSuccess)
{
  EXPECT_TRUE(factory.install(WidgetItem::classId(), widgetCreator()));
}

TEST_F(ObjectFactoryTest, InstallReturnsFalseWhenAlreadyInstalled)
{
  factory.install(WidgetItem::classId(), widgetCreator());
  EXPECT_FALSE(factory.install(WidgetItem::classId(), widgetCreator()));
}

TEST_F(ObjectFactoryTest, InstallSameTypeIdTwiceIsIdempotentCount)
{
  factory.install(WidgetItem::classId(), widgetCreator());
  factory.install(WidgetItem::classId(), widgetCreator());
  EXPECT_EQ(factory.installedCount(), 1);
}

TEST_F(ObjectFactoryTest, IsInstalledReturnsTrueAfterInstall)
{
  factory.install(WidgetItem::classId(), widgetCreator());
  EXPECT_TRUE(factory.isInstalled(WidgetItem::classId()));
}

TEST_F(ObjectFactoryTest, IsInstalledReturnsFalseForUnknownType)
{
  EXPECT_FALSE(factory.isInstalled(WidgetItem::classId()));
}

// ── uninstall ─────────────────────────────────────────────────────────────────

TEST_F(ObjectFactoryTest, UninstallDecreasesCount)
{
  factory.install(WidgetItem::classId(), widgetCreator());
  factory.uninstall(WidgetItem::classId());
  EXPECT_EQ(factory.installedCount(), 0);
}

TEST_F(ObjectFactoryTest, UninstallReturnsTrueOnSuccess)
{
  factory.install(WidgetItem::classId(), widgetCreator());
  EXPECT_TRUE(factory.uninstall(WidgetItem::classId()));
}

TEST_F(ObjectFactoryTest, UninstallReturnsFalseForUnknownType)
{
  EXPECT_FALSE(factory.uninstall(WidgetItem::classId()));
}

TEST_F(ObjectFactoryTest, UninstalledTypeIsNoLongerInstalled)
{
  factory.install(WidgetItem::classId(), widgetCreator());
  factory.uninstall(WidgetItem::classId());
  EXPECT_FALSE(factory.isInstalled(WidgetItem::classId()));
}

TEST_F(ObjectFactoryTest, UninstallOnlyAffectsTargetType)
{
  factory.install(WidgetItem::classId(), widgetCreator());
  factory.install(GadgetItem::classId(), gadgetCreator());
  factory.uninstall(WidgetItem::classId());

  EXPECT_FALSE(factory.isInstalled(WidgetItem::classId()));
  EXPECT_TRUE(factory.isInstalled(GadgetItem::classId()));
}

// ── installedTypes ────────────────────────────────────────────────────────────

TEST_F(ObjectFactoryTest, InstalledTypesContainsRegisteredTypeIds)
{
  factory.install(WidgetItem::classId(), widgetCreator());
  factory.install(GadgetItem::classId(), gadgetCreator());

  auto const types = factory.installedTypes();
  EXPECT_THAT(types, ::testing::UnorderedElementsAre(WidgetItem::classId(), GadgetItem::classId()));
}

TEST_F(ObjectFactoryTest, InstalledTypesDoesNotContainUninstalledType)
{
  factory.install(WidgetItem::classId(), widgetCreator());
  factory.install(GadgetItem::classId(), gadgetCreator());
  factory.uninstall(WidgetItem::classId());

  auto const types = factory.installedTypes();
  EXPECT_THAT(types, ::testing::UnorderedElementsAre(GadgetItem::classId()));
}

// ── create with explicit objectId ─────────────────────────────────────────────

TEST_F(ObjectFactoryTest, CreateReturnsNonNullForInstalledType)
{
  factory.install(WidgetItem::classId(), widgetCreator());
  auto const obj = factory.create(WidgetItem::classId(), boost::uuids::random_generator{}());
  EXPECT_NE(obj, nullptr);
}

TEST_F(ObjectFactoryTest, CreateReturnsNullptrForUnknownType)
{
  auto const obj = factory.create(WidgetItem::classId(), boost::uuids::random_generator{}());
  EXPECT_EQ(obj, nullptr);
}

TEST_F(ObjectFactoryTest, CreatePreservesExplicitObjectId)
{
  factory.install(WidgetItem::classId(), widgetCreator());
  auto const desiredId = boost::uuids::random_generator{}();
  auto const obj = factory.create(WidgetItem::classId(), desiredId);

  ASSERT_NE(obj, nullptr);
  EXPECT_EQ(obj->objectId(), desiredId);
}

TEST_F(ObjectFactoryTest, CreateReturnsCorrectSubtype)
{
  factory.install(WidgetItem::classId(), widgetCreator());
  auto const obj = factory.create(WidgetItem::classId(), boost::uuids::random_generator{}());

  ASSERT_NE(obj, nullptr);
  EXPECT_EQ(obj->typeId(), WidgetItem::classId());
}

TEST_F(ObjectFactoryTest, CreateTwoObjectsWithSameTypeHaveDifferentObjectIds)
{
  factory.install(WidgetItem::classId(), widgetCreator());
  auto const id1 = boost::uuids::random_generator{}();
  auto const id2 = boost::uuids::random_generator{}();
  auto const a = factory.create(WidgetItem::classId(), id1);
  auto const b = factory.create(WidgetItem::classId(), id2);

  ASSERT_NE(a, nullptr);
  ASSERT_NE(b, nullptr);
  EXPECT_NE(a->objectId(), b->objectId());
}

// ── create with auto-generated objectId ──────────────────────────────────────

TEST_F(ObjectFactoryTest, CreateWithoutObjectIdReturnsNonNull)
{
  factory.install(WidgetItem::classId(), widgetCreator());
  EXPECT_NE(factory.create(WidgetItem::classId()), nullptr);
}

TEST_F(ObjectFactoryTest, CreateWithoutObjectIdGeneratesNonNilId)
{
  factory.install(WidgetItem::classId(), widgetCreator());
  auto const obj = factory.create(WidgetItem::classId());
  ASSERT_NE(obj, nullptr);
  EXPECT_FALSE(obj->objectId().is_nil());
}

TEST_F(ObjectFactoryTest, TwoAutoCreatedObjectsHaveDifferentObjectIds)
{
  factory.install(WidgetItem::classId(), widgetCreator());
  auto const a = factory.create(WidgetItem::classId());
  auto const b = factory.create(WidgetItem::classId());

  ASSERT_NE(a, nullptr);
  ASSERT_NE(b, nullptr);
  EXPECT_NE(a->objectId(), b->objectId());
}

TEST_F(ObjectFactoryTest, AutoCreateReturnsNullptrForUnknownType)
{
  EXPECT_EQ(factory.create(WidgetItem::classId()), nullptr);
}
