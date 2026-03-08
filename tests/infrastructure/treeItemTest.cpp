#include <treeItem.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid.hpp>

#include <QCoreApplication>

// ── Qt requires a QCoreApplication for signal/slot delivery ──────────────────
namespace {
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
int s_argc = 0;
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
QCoreApplication s_app{s_argc, nullptr};
} // namespace

using namespace infrastructure;
using ::testing::_;
using ::testing::InSequence;

// ── Mock observer (plain struct, connected to signals via lambdas) ─────────────

struct MockTreeObserver
{
  MOCK_METHOD(void, onChildAboutToBeInserted, (int index, std::shared_ptr<TreeItem> child));
  MOCK_METHOD(void, onChildInserted, (int index, std::shared_ptr<TreeItem> child));
  MOCK_METHOD(void, onChildAboutToBeRemoved, (int index, std::shared_ptr<TreeItem> child));
  MOCK_METHOD(void, onChildRemoved, (int index, std::shared_ptr<TreeItem> child));

  MOCK_METHOD(void, onSubtreeChildAboutToBeInserted,
              (std::shared_ptr<TreeItem> parent, int index, std::shared_ptr<TreeItem> child));
  MOCK_METHOD(void, onSubtreeChildInserted,
              (std::shared_ptr<TreeItem> parent, int index, std::shared_ptr<TreeItem> child));
  MOCK_METHOD(void, onSubtreeChildAboutToBeRemoved,
              (std::shared_ptr<TreeItem> parent, int index, std::shared_ptr<TreeItem> child));
  MOCK_METHOD(void, onSubtreeChildRemoved,
              (std::shared_ptr<TreeItem> parent, int index, std::shared_ptr<TreeItem> child));
};

// ── Helper: connect all local signals of a node to a mock observer ─────────────

auto connectLocalSignals(std::shared_ptr<TreeItem> const& node, MockTreeObserver& obs) -> void
{
  QObject::connect(node.get(), &TreeItem::childAboutToBeInserted,
                   [&obs](int idx, auto child) { obs.onChildAboutToBeInserted(idx, child); });
  QObject::connect(node.get(), &TreeItem::childInserted,
                   [&obs](int idx, auto child) { obs.onChildInserted(idx, child); });
  QObject::connect(node.get(), &TreeItem::childAboutToBeRemoved,
                   [&obs](int idx, auto child) { obs.onChildAboutToBeRemoved(idx, child); });
  QObject::connect(node.get(), &TreeItem::childRemoved,
                   [&obs](int idx, auto child) { obs.onChildRemoved(idx, child); });
}

auto connectSubtreeSignals(std::shared_ptr<TreeItem> const& node, MockTreeObserver& obs) -> void
{
  QObject::connect(node.get(), &TreeItem::subtreeChildAboutToBeInserted,
                   [&obs](auto parent, int idx, auto child) {
                     obs.onSubtreeChildAboutToBeInserted(parent, idx, child);
                   });
  QObject::connect(
      node.get(), &TreeItem::subtreeChildInserted,
      [&obs](auto parent, int idx, auto child) { obs.onSubtreeChildInserted(parent, idx, child); });
  QObject::connect(node.get(), &TreeItem::subtreeChildAboutToBeRemoved,
                   [&obs](auto parent, int idx, auto child) {
                     obs.onSubtreeChildAboutToBeRemoved(parent, idx, child);
                   });
  QObject::connect(
      node.get(), &TreeItem::subtreeChildRemoved,
      [&obs](auto parent, int idx, auto child) { obs.onSubtreeChildRemoved(parent, idx, child); });
}

// ── Test fixture ────────────────────────────────────────────────────────────────

class TreeItemTest : public ::testing::Test
{
protected:
  // root → child1, child2; child1 → grandchild
  std::shared_ptr<TreeItem> root = std::make_shared<TreeItem>();
  std::shared_ptr<TreeItem> child1 = std::make_shared<TreeItem>();
  std::shared_ptr<TreeItem> child2 = std::make_shared<TreeItem>();
  std::shared_ptr<TreeItem> grandchild = std::make_shared<TreeItem>();
};

// ── Initial state ───────────────────────────────────────────────────────────────

TEST_F(TreeItemTest, InitialStateIsEmpty)
{
  EXPECT_EQ(root->size(), 0);
  EXPECT_EQ(root->begin(), root->end());
  EXPECT_TRUE(root->parentItem().expired());
}

// ── insertChild ─────────────────────────────────────────────────────────────────

TEST_F(TreeItemTest, InsertChildAppend)
{
  root->insertChild(0, child1);
  root->insertChild(1, child2);

  EXPECT_EQ(root->size(), 2);
  EXPECT_EQ(root->childAt(0), child1);
  EXPECT_EQ(root->childAt(1), child2);
}

TEST_F(TreeItemTest, InsertChildAtFront)
{
  root->insertChild(0, child2);
  root->insertChild(0, child1);

  EXPECT_EQ(root->childAt(0), child1);
  EXPECT_EQ(root->childAt(1), child2);
}

TEST_F(TreeItemTest, InsertChildIndexClampsToEnd)
{
  root->insertChild(999, child1);

  EXPECT_EQ(root->size(), 1);
  EXPECT_EQ(root->childAt(0), child1);
}

TEST_F(TreeItemTest, InsertChildSetsParent)
{
  root->insertChild(0, child1);

  EXPECT_EQ(child1->parentItem().lock(), root);
}

// ── removeChild ─────────────────────────────────────────────────────────────────

TEST_F(TreeItemTest, RemoveChildDecreasesSizeAndClearsParent)
{
  root->insertChild(0, child1);
  root->removeChild(child1);

  EXPECT_EQ(root->size(), 0);
  EXPECT_TRUE(child1->parentItem().expired());
}

TEST_F(TreeItemTest, RemoveChildNoOpForNonChild)
{
  root->insertChild(0, child1);
  root->removeChild(child2); // child2 was never added

  EXPECT_EQ(root->size(), 1);
}

TEST_F(TreeItemTest, RemoveChildMiddleElement)
{
  root->insertChild(0, child1);
  root->insertChild(1, child2);
  root->removeChild(child1);

  EXPECT_EQ(root->size(), 1);
  EXPECT_EQ(root->childAt(0), child2);
}

// ── Range iteration ─────────────────────────────────────────────────────────────

TEST_F(TreeItemTest, RangeBasedForIteratesChildren)
{
  root->insertChild(0, child1);
  root->insertChild(1, child2);

  std::vector<std::shared_ptr<TreeItem>> collected;
  for (auto const& c : *root) {
    collected.push_back(c);
  }

  EXPECT_THAT(collected, ::testing::ElementsAre(child1, child2));
}

// ── Local signals – insert ───────────────────────────────────────────────────────

TEST_F(TreeItemTest, InsertChildEmitsLocalSignalsInOrder)
{
  MockTreeObserver obs;
  connectLocalSignals(root, obs);

  InSequence seq;
  EXPECT_CALL(obs, onChildAboutToBeInserted(0, child1));
  EXPECT_CALL(obs, onChildInserted(0, child1));

  root->insertChild(0, child1);
}

TEST_F(TreeItemTest, AboutToBeInsertedFiresBeforeStateChange)
{
  // When aboutToBeInserted fires, the child must not yet be in the list.
  connectLocalSignals(root, *new MockTreeObserver()); // ignored mock
  bool sizeWasZeroInAboutTo = false;

  QObject::connect(
      root.get(), &TreeItem::childAboutToBeInserted,
      [&](int, std::shared_ptr<TreeItem>) { sizeWasZeroInAboutTo = (root->size() == 0); });

  root->insertChild(0, child1);

  EXPECT_TRUE(sizeWasZeroInAboutTo);
}

TEST_F(TreeItemTest, ChildInsertedFiresAfterStateChange)
{
  bool sizeWasOneInInserted = false;

  QObject::connect(root.get(), &TreeItem::childInserted, [&](int, std::shared_ptr<TreeItem>) {
    sizeWasOneInInserted = (root->size() == 1);
  });

  root->insertChild(0, child1);

  EXPECT_TRUE(sizeWasOneInInserted);
}

// ── Local signals – remove ───────────────────────────────────────────────────────

TEST_F(TreeItemTest, RemoveChildEmitsLocalSignalsInOrder)
{
  root->insertChild(0, child1);

  MockTreeObserver obs;
  connectLocalSignals(root, obs);

  InSequence seq;
  EXPECT_CALL(obs, onChildAboutToBeRemoved(0, child1));
  EXPECT_CALL(obs, onChildRemoved(0, child1));

  root->removeChild(child1);
}

TEST_F(TreeItemTest, AboutToBeRemovedFiresBeforeStateChange)
{
  root->insertChild(0, child1);

  bool childStillPresentInAboutTo = false;
  QObject::connect(
      root.get(), &TreeItem::childAboutToBeRemoved,
      [&](int, std::shared_ptr<TreeItem>) { childStillPresentInAboutTo = (root->size() == 1); });

  root->removeChild(child1);

  EXPECT_TRUE(childStillPresentInAboutTo);
}

TEST_F(TreeItemTest, ChildRemovedFiresAfterStateChange)
{
  root->insertChild(0, child1);

  bool childGoneInRemoved = false;
  QObject::connect(root.get(), &TreeItem::childRemoved, [&](int, std::shared_ptr<TreeItem>) {
    childGoneInRemoved = (root->size() == 0);
  });

  root->removeChild(child1);

  EXPECT_TRUE(childGoneInRemoved);
}

TEST_F(TreeItemTest, NoSignalsEmittedWhenRemovingNonChild)
{
  MockTreeObserver obs;
  connectLocalSignals(root, obs);

  EXPECT_CALL(obs, onChildAboutToBeRemoved(_, _)).Times(0);
  EXPECT_CALL(obs, onChildRemoved(_, _)).Times(0);

  root->removeChild(child1); // child1 was never added
}

// ── Subtree signals ─────────────────────────────────────────────────────────────

TEST_F(TreeItemTest, SubtreeSignalsPropagateToGrandparent)
{
  root->insertChild(0, child1);

  MockTreeObserver obs;
  connectSubtreeSignals(root, obs);

  InSequence seq;
  EXPECT_CALL(obs, onSubtreeChildAboutToBeInserted(child1, 0, grandchild));
  EXPECT_CALL(obs, onSubtreeChildInserted(child1, 0, grandchild));

  child1->insertChild(0, grandchild);
}

TEST_F(TreeItemTest, SubtreeSignalsCarryCorrectParent)
{
  root->insertChild(0, child1);

  std::shared_ptr<TreeItem> capturedParent;
  QObject::connect(root.get(), &TreeItem::subtreeChildInserted,
                   [&](auto parent, int, auto) { capturedParent = parent; });

  child1->insertChild(0, grandchild);

  EXPECT_EQ(capturedParent, child1);
}

TEST_F(TreeItemTest, SubtreeRemoveSignalsPropagateToGrandparent)
{
  root->insertChild(0, child1);
  child1->insertChild(0, grandchild);

  MockTreeObserver obs;
  connectSubtreeSignals(root, obs);

  InSequence seq;
  EXPECT_CALL(obs, onSubtreeChildAboutToBeRemoved(child1, 0, grandchild));
  EXPECT_CALL(obs, onSubtreeChildRemoved(child1, 0, grandchild));

  child1->removeChild(grandchild);
}

TEST_F(TreeItemTest, SubtreeSignalsDisconnectedAfterRemoveChild)
{
  root->insertChild(0, child1);
  root->removeChild(child1);

  MockTreeObserver obs;
  connectSubtreeSignals(root, obs);

  // grandchild insertion into detached child1 must not reach root
  EXPECT_CALL(obs, onSubtreeChildInserted(_, _, _)).Times(0);

  child1->insertChild(0, grandchild);
}

TEST_F(TreeItemTest, LocalSignalsNotFiredForSubtreeChanges)
{
  root->insertChild(0, child1);

  MockTreeObserver obs;
  connectLocalSignals(root, obs);

  // Local insert/remove signals should NOT fire when grandchild is added
  EXPECT_CALL(obs, onChildAboutToBeInserted(_, _)).Times(0);
  EXPECT_CALL(obs, onChildInserted(_, _)).Times(0);

  child1->insertChild(0, grandchild);
}

// ── Identity ────────────────────────────────────────────────────────────────────

// Helper subclass to verify typeId() override pattern
class ConcreteItem : public TreeItem
{
public:
  using TreeItem::TreeItem;

  static auto classId() -> boost::uuids::uuid
  {
    static boost::uuids::uuid const id =
        boost::uuids::string_generator{}("b1234567-89ab-cdef-0123-456789abcdef");
    return id;
  }

  auto typeId() const -> boost::uuids::uuid override { return ConcreteItem::classId(); }
};

TEST_F(TreeItemTest, ClassIdIsStable) { EXPECT_EQ(TreeItem::classId(), TreeItem::classId()); }

TEST_F(TreeItemTest, ClassIdIsNonNil) { EXPECT_NE(TreeItem::classId(), boost::uuids::nil_uuid()); }

TEST_F(TreeItemTest, TypeIdMatchesClassIdForBaseType)
{
  EXPECT_EQ(root->typeId(), TreeItem::classId());
}

TEST_F(TreeItemTest, DefaultConstructorGeneratesNonNilObjectId)
{
  EXPECT_NE(root->objectId(), boost::uuids::nil_uuid());
}

TEST_F(TreeItemTest, TwoInstancesHaveDifferentObjectIds)
{
  EXPECT_NE(root->objectId(), child1->objectId());
}

TEST_F(TreeItemTest, ExplicitObjectIdIsPreserved)
{
  auto const id = boost::uuids::random_generator{}();
  auto item = std::make_shared<TreeItem>(id);
  EXPECT_EQ(item->objectId(), id);
}

TEST_F(TreeItemTest, ObjectIdIsStableAcrossCalls)
{
  auto const id1 = root->objectId();
  auto const id2 = root->objectId();
  EXPECT_EQ(id1, id2);
}

TEST_F(TreeItemTest, SubclassTypeIdDiffersFromBaseClassId)
{
  auto concrete = std::make_shared<ConcreteItem>();
  EXPECT_NE(concrete->typeId(), TreeItem::classId());
  EXPECT_EQ(concrete->typeId(), ConcreteItem::classId());
}

TEST_F(TreeItemTest, SubclassClassIdIsStable)
{
  EXPECT_EQ(ConcreteItem::classId(), ConcreteItem::classId());
}
