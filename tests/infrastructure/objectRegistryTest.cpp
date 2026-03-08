#include <objectRegistry.hpp>
#include <treeItem.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>

// QCoreApplication is provided by treeItemTest.cpp (same test executable).

using namespace infrastructure;

// ── Fixture ──────────────────────────────────────────────────────────────────

class ObjectRegistryTest : public ::testing::Test
{
protected:
  ObjectRegistry registry;

  std::shared_ptr<TreeItem> item1 = std::make_shared<TreeItem>();
  std::shared_ptr<TreeItem> item2 = std::make_shared<TreeItem>();
};

// ── Initial state ─────────────────────────────────────────────────────────────

TEST_F(ObjectRegistryTest, InitiallyEmpty) { EXPECT_EQ(registry.registeredCount(), 0); }

TEST_F(ObjectRegistryTest, FindOnEmptyRegistryReturnsNull)
{
  EXPECT_TRUE(registry.findObject(item1->objectId()).expired());
}

// ── registerObject ────────────────────────────────────────────────────────────

TEST_F(ObjectRegistryTest, RegisterIncreasesCount)
{
  registry.registerObject(item1);
  EXPECT_EQ(registry.registeredCount(), 1);
}

TEST_F(ObjectRegistryTest, RegisterTwoItemsCountIsTwo)
{
  registry.registerObject(item1);
  registry.registerObject(item2);
  EXPECT_EQ(registry.registeredCount(), 2);
}

TEST_F(ObjectRegistryTest, RegisteredObjectCanBeFound)
{
  registry.registerObject(item1);
  EXPECT_EQ(registry.findObject(item1->objectId()).lock(), item1);
}

TEST_F(ObjectRegistryTest, RegisterSameObjectTwiceIsIdempotent)
{
  registry.registerObject(item1);
  registry.registerObject(item1);
  EXPECT_EQ(registry.registeredCount(), 1);
}

TEST_F(ObjectRegistryTest, FindUnregisteredIdReturnsNull)
{
  registry.registerObject(item1);
  EXPECT_TRUE(registry.findObject(item2->objectId()).expired());
}

// ── unregisterObject ──────────────────────────────────────────────────────────

TEST_F(ObjectRegistryTest, UnregisterDecreasesCount)
{
  registry.registerObject(item1);
  registry.unregisterObject(item1->objectId());
  EXPECT_EQ(registry.registeredCount(), 0);
}

TEST_F(ObjectRegistryTest, UnregisteredObjectCanNoLongerBeFound)
{
  registry.registerObject(item1);
  registry.unregisterObject(item1->objectId());
  EXPECT_TRUE(registry.findObject(item1->objectId()).expired());
}

TEST_F(ObjectRegistryTest, UnregisterNonExistentIdIsNoOp)
{
  registry.registerObject(item1);
  registry.unregisterObject(item2->objectId()); // item2 was never registered
  EXPECT_EQ(registry.registeredCount(), 1);
}

TEST_F(ObjectRegistryTest, UnregisterOnlyAffectsTargetItem)
{
  registry.registerObject(item1);
  registry.registerObject(item2);
  registry.unregisterObject(item1->objectId());

  EXPECT_TRUE(registry.findObject(item1->objectId()).expired());
  EXPECT_EQ(registry.findObject(item2->objectId()).lock(), item2);
}

// ── Auto-removal on destruction ───────────────────────────────────────────────

TEST_F(ObjectRegistryTest, DestroyedObjectIsRemovedAutomatically)
{
  // Register a temporary item, then let it go out of scope
  {
    auto tempItem = std::make_shared<TreeItem>();
    registry.registerObject(tempItem);
    ASSERT_EQ(registry.registeredCount(), 1);
  }
  // shared_ptr released → QObject::destroyed fires → entry removed
  EXPECT_EQ(registry.registeredCount(), 0);
}

TEST_F(ObjectRegistryTest, DestroyedObjectWeakPtrIsExpired)
{
  boost::uuids::uuid capturedId;
  {
    auto tempItem = std::make_shared<TreeItem>();
    capturedId = tempItem->objectId();
    registry.registerObject(tempItem);
  }
  EXPECT_TRUE(registry.findObject(capturedId).expired());
}

TEST_F(ObjectRegistryTest, OtherItemsUntouchedAfterSiblingDestroyed)
{
  registry.registerObject(item1);
  {
    auto tempItem = std::make_shared<TreeItem>();
    registry.registerObject(tempItem);
  }
  // Only the temp item is gone — item1 must still be findable
  EXPECT_EQ(registry.findObject(item1->objectId()).lock(), item1);
  EXPECT_EQ(registry.registeredCount(), 1);
}

TEST_F(ObjectRegistryTest, ExplicitUnregisterSuppressesAutoRemoveOnDestruction)
{
  // Unregister before the object is destroyed — count must not go negative
  auto tempItem = std::make_shared<TreeItem>();
  registry.registerObject(tempItem);
  registry.unregisterObject(tempItem->objectId());
  ASSERT_EQ(registry.registeredCount(), 0);

  // Destroying the object now must not affect the empty registry
  tempItem.reset();
  EXPECT_EQ(registry.registeredCount(), 0);
}

// ── Registry does not extend object lifetime ──────────────────────────────────

TEST_F(ObjectRegistryTest, RegistryDoesNotKeepObjectAlive)
{
  std::weak_ptr<TreeItem> weak;
  {
    auto tempItem = std::make_shared<TreeItem>();
    weak = tempItem;
    registry.registerObject(tempItem);
  }
  // No other shared_ptr holds the object → it must be destroyed
  EXPECT_TRUE(weak.expired());
}
