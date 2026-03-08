#include <itemResolver.hpp>
#include <objectRegistry.hpp>
#include <treeItem.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <QCoreApplication>

// QCoreApplication instance provided by treeItemTest.cpp (same executable).

using namespace infrastructure;

// ── Helpers ───────────────────────────────────────────────────────────────────

namespace {

// Pump the event loop so queued connections fire.
void processEvents() { QCoreApplication::processEvents(QEventLoop::AllEvents); }

} // namespace

// ── Fixture ───────────────────────────────────────────────────────────────────

class ItemResolverTest : public ::testing::Test
{
protected:
  ObjectRegistry registry;

  // A real target item and a root to act as parent of the resolver.
  std::shared_ptr<TreeItem> root = std::make_shared<TreeItem>();
  std::shared_ptr<TreeItem> target = std::make_shared<TreeItem>();

  // Helper: build a resolver watching for target->objectId()
  auto makeResolver() -> std::shared_ptr<ItemResolver>
  {
    return std::make_shared<ItemResolver>(target->objectId(), registry);
  }
};

// ── Construction ──────────────────────────────────────────────────────────────

TEST_F(ItemResolverTest, NotResolvedAtConstruction)
{
  auto resolver = makeResolver();
  EXPECT_FALSE(resolver->isResolved());
}

TEST_F(ItemResolverTest, TargetIdMatchesRequested)
{
  auto resolver = makeResolver();
  EXPECT_EQ(resolver->targetId(), target->objectId());
}

TEST_F(ItemResolverTest, ClassIdIsNonNil) { EXPECT_FALSE(ItemResolver::classId().is_nil()); }

TEST_F(ItemResolverTest, TypeIdMatchesClassId)
{
  auto resolver = makeResolver();
  EXPECT_EQ(resolver->typeId(), ItemResolver::classId());
}

TEST_F(ItemResolverTest, ClassIdDiffersFromTreeItemClassId)
{
  EXPECT_NE(ItemResolver::classId(), TreeItem::classId());
}

// ── Not in tree ───────────────────────────────────────────────────────────────

TEST_F(ItemResolverTest, RegisteringTargetWhileNotInTreeDoesNotResolve)
{
  auto resolver = makeResolver();
  registry.registerObject(target);
  processEvents();
  EXPECT_FALSE(resolver->isResolved());
}

// ── Resolves when target already registered ───────────────────────────────────

TEST_F(ItemResolverTest, ResolvesWhenInsertedAndTargetAlreadyRegistered)
{
  registry.registerObject(target);
  auto resolver = makeResolver();
  root->insertChild(0, resolver);
  processEvents();

  EXPECT_TRUE(resolver->isResolved());
}

TEST_F(ItemResolverTest, ResolverIsRemovedFromTreeAfterResolution)
{
  registry.registerObject(target);
  auto resolver = makeResolver();
  root->insertChild(0, resolver);
  processEvents();

  EXPECT_EQ(resolver->parentItem().lock(), nullptr);
}

TEST_F(ItemResolverTest, TargetOccupiesTreeSlotAfterResolution)
{
  registry.registerObject(target);
  auto resolver = makeResolver();
  root->insertChild(0, resolver);
  processEvents();

  ASSERT_EQ(root->size(), 1);
  EXPECT_EQ(root->childAt(0), target);
}

// ── Resolves when target registered after insertion ───────────────────────────

TEST_F(ItemResolverTest, ResolvesWhenTargetRegisteredAfterInsertion)
{
  auto resolver = makeResolver();
  root->insertChild(0, resolver);
  EXPECT_FALSE(resolver->isResolved());

  registry.registerObject(target);
  processEvents();

  EXPECT_TRUE(resolver->isResolved());
}

TEST_F(ItemResolverTest, TargetOccupiesSlotWhenRegisteredAfterInsertion)
{
  auto resolver = makeResolver();
  root->insertChild(0, resolver);
  registry.registerObject(target);
  processEvents();

  ASSERT_EQ(root->size(), 1);
  EXPECT_EQ(root->childAt(0), target);
}

// ── Position preservation ─────────────────────────────────────────────────────

TEST_F(ItemResolverTest, TargetInsertedAtSameIndexAsResolver)
{
  // Put two siblings around the resolver so there's a meaningful index to check.
  auto before = std::make_shared<TreeItem>();
  auto after = std::make_shared<TreeItem>();
  root->insertChild(0, before); // index 0
  auto resolver = makeResolver();
  root->insertChild(1, resolver); // index 1
  root->insertChild(2, after);    // index 2

  registry.registerObject(target);
  processEvents();

  ASSERT_EQ(root->size(), 3);
  EXPECT_EQ(root->childAt(0), before);
  EXPECT_EQ(root->childAt(1), target); // resolver replaced at its original index
  EXPECT_EQ(root->childAt(2), after);
}

// ── Resolution is idempotent ──────────────────────────────────────────────────

TEST_F(ItemResolverTest, RegisteringTargetTwiceDoesNotCrash)
{
  auto resolver = makeResolver();
  root->insertChild(0, resolver);
  registry.registerObject(target);
  processEvents();

  // Re-registering (idempotent in registry) must not cause a second swap
  registry.unregisterObject(target->objectId());
  registry.registerObject(target);
  processEvents();

  ASSERT_EQ(root->size(), 1);
  EXPECT_EQ(root->childAt(0), target);
}

// ── Unrelated registrations are ignored ──────────────────────────────────────

TEST_F(ItemResolverTest, UnrelatedRegistrationDoesNotResolve)
{
  auto resolver = makeResolver();
  auto unrelated = std::make_shared<TreeItem>();
  root->insertChild(0, resolver);

  registry.registerObject(unrelated);
  processEvents();

  EXPECT_FALSE(resolver->isResolved());
  EXPECT_EQ(root->childAt(0), resolver);
}

// ── Target parent is set correctly after resolution ───────────────────────────

TEST_F(ItemResolverTest, TargetParentIsRootAfterResolution)
{
  registry.registerObject(target);
  auto resolver = makeResolver();
  root->insertChild(0, resolver);
  processEvents();

  EXPECT_EQ(target->parentItem().lock(), root);
}

// ── Resolver has no parent after resolution ───────────────────────────────────

TEST_F(ItemResolverTest, ResolverParentIsNullAfterResolution)
{
  registry.registerObject(target);
  auto resolver = makeResolver();
  root->insertChild(0, resolver);
  processEvents();

  EXPECT_EQ(resolver->parentItem().lock(), nullptr);
}

// ── No memory leak: resolver is destroyed after resolution ────────────────────

TEST_F(ItemResolverTest, ResolverIsDestroyedAfterResolutionWhenCallerReleasesIt)
{
  // Keep only a weak observer – the tree must not hold the resolver alive after
  // swapping it out, and this test's local shared_ptr is the only other owner.
  registry.registerObject(target);
  std::weak_ptr<ItemResolver> weak;
  {
    auto resolver = makeResolver();
    weak = resolver;
    root->insertChild(0, resolver);
    processEvents(); // swap fires; tree now holds target, not resolver
    // resolver shared_ptr goes out of scope here
  }
  // At this point the only potential owner was the local shared_ptr we just
  // released. The tree no longer references the resolver, so it must be gone.
  EXPECT_TRUE(weak.expired());
}
