#include "skillDefault.hpp"

#include "parentRef.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

// ── Signal observer ───────────────────────────────────────────────────────────

namespace {

struct MockSkillDefaultObserver
{
  MOCK_METHOD(void, onModifierChanged, (int));
  MOCK_METHOD(void, onTargetChanged, ());
};

} // namespace

using namespace gurps_system;

// ── Fixture ───────────────────────────────────────────────────────────────────

class SkillDefaultTest : public ::testing::Test
{
protected:
  std::shared_ptr<SkillDefault> skillDefault;

  void SetUp() override { skillDefault = std::make_shared<SkillDefault>(); }
};

// ── Identity ──────────────────────────────────────────────────────────────────

TEST_F(SkillDefaultTest, ClassIdIsNonNil) { EXPECT_FALSE(SkillDefault::classId().is_nil()); }

TEST_F(SkillDefaultTest, ClassIdIsStable)
{
  EXPECT_EQ(SkillDefault::classId(), SkillDefault::classId());
}

TEST_F(SkillDefaultTest, TypeIdMatchesClassId)
{
  EXPECT_EQ(skillDefault->typeId(), SkillDefault::classId());
}

TEST_F(SkillDefaultTest, ObjectIdIsNonNil) { EXPECT_FALSE(skillDefault->objectId().is_nil()); }

// ── Default state ─────────────────────────────────────────────────────────────

TEST_F(SkillDefaultTest, DefaultModifierIsZero) { EXPECT_EQ(skillDefault->modifier(), 0); }

TEST_F(SkillDefaultTest, DefaultTargetRefIsNull) { EXPECT_EQ(skillDefault->target(), nullptr); }

// ── modifier property ─────────────────────────────────────────────────────────

TEST_F(SkillDefaultTest, SetModifierUpdatesModifier)
{
  skillDefault->setModifier(-4);
  EXPECT_EQ(skillDefault->modifier(), -4);
}

TEST_F(SkillDefaultTest, SetModifierEmitsModifierChanged)
{
  MockSkillDefaultObserver observer;
  QObject::connect(skillDefault.get(), &SkillDefault::modifierChanged,
                   [&observer](int v) { observer.onModifierChanged(v); });

  EXPECT_CALL(observer, onModifierChanged(-4)).Times(1);
  skillDefault->setModifier(-4);
}

TEST_F(SkillDefaultTest, SetModifierToSameValueDoesNotEmitSignal)
{
  skillDefault->setModifier(-4);

  MockSkillDefaultObserver observer;
  QObject::connect(skillDefault.get(), &SkillDefault::modifierChanged,
                   [&observer](int v) { observer.onModifierChanged(v); });

  EXPECT_CALL(observer, onModifierChanged(::testing::_)).Times(0);
  skillDefault->setModifier(-4);
}

TEST_F(SkillDefaultTest, SetModifierToNegativeValue)
{
  skillDefault->setModifier(-10);
  EXPECT_EQ(skillDefault->modifier(), -10);
}

TEST_F(SkillDefaultTest, SetModifierToPositiveValue)
{
  skillDefault->setModifier(5);
  EXPECT_EQ(skillDefault->modifier(), 5);
}

// ── target reference ──────────────────────────────────────────────────────────

TEST_F(SkillDefaultTest, SetTargetSetsFirstChild)
{
  auto ref = std::make_shared<ParentRef>();
  skillDefault->setTarget(ref.get());

  EXPECT_EQ(skillDefault->size(), 1);
  EXPECT_EQ(skillDefault->childAt(0), ref);
}

TEST_F(SkillDefaultTest, SetTargetEmitsTargetChanged)
{
  auto ref = std::make_shared<ParentRef>();

  MockSkillDefaultObserver observer;
  QObject::connect(skillDefault.get(), &SkillDefault::targetChanged,
                   [&observer]() { observer.onTargetChanged(); });

  EXPECT_CALL(observer, onTargetChanged()).Times(1);
  skillDefault->setTarget(ref.get());
}

TEST_F(SkillDefaultTest, SetTargetReplacesExistingRef)
{
  auto ref1 = std::make_shared<ParentRef>();
  auto ref2 = std::make_shared<ParentRef>();

  skillDefault->setTarget(ref1.get());
  skillDefault->setTarget(ref2.get());

  EXPECT_EQ(skillDefault->size(), 1);
  EXPECT_EQ(skillDefault->childAt(0), ref2);
}

TEST_F(SkillDefaultTest, SetTargetToNullRemovesChild)
{
  auto ref = std::make_shared<ParentRef>();
  skillDefault->setTarget(ref.get());

  skillDefault->setTarget(nullptr);

  EXPECT_EQ(skillDefault->size(), 0);
}

TEST_F(SkillDefaultTest, TargetRefAccessibleViaTreeItemInterface)
{
  auto ref = std::make_shared<ParentRef>();
  skillDefault->setTarget(ref.get());

  infrastructure::TreeItem* item = skillDefault.get();
  EXPECT_EQ(item->size(), 1);
  EXPECT_EQ(item->childAt(0), ref);
}

TEST_F(SkillDefaultTest, TargetRefIsFirstChild)
{
  auto ref = std::make_shared<ParentRef>();
  skillDefault->setTarget(ref.get());

  EXPECT_EQ(skillDefault->target(), ref.get());
}

TEST_F(SkillDefaultTest, CanSetAllProperties)
{
  auto ref = std::make_shared<ParentRef>();
  skillDefault->setModifier(-5);
  skillDefault->setTarget(ref.get());

  EXPECT_EQ(skillDefault->modifier(), -5);
  EXPECT_EQ(skillDefault->target(), ref.get());
}
