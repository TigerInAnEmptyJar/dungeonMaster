#include <skill.hpp>

#include "parentRef.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <QCoreApplication>

// ── Signal observer ───────────────────────────────────────────────────────────

namespace {

struct MockSkillObserver
{
  MOCK_METHOD(void, onDifficultyChanged, (int));
  MOCK_METHOD(void, onTypeChanged, (gurps_system::Skill::SkillType));
  MOCK_METHOD(void, onAttributeRefChanged, ());
};

} // namespace

using namespace gurps_system;

// ── Fixture ───────────────────────────────────────────────────────────────────

class SkillTest : public ::testing::Test
{
protected:
  std::shared_ptr<Skill> skill;

  void SetUp() override { skill = std::make_shared<Skill>(); }
};

// ── Identity ──────────────────────────────────────────────────────────────────

TEST_F(SkillTest, ClassIdIsNonNil) { EXPECT_FALSE(Skill::classId().is_nil()); }

TEST_F(SkillTest, ClassIdIsStable) { EXPECT_EQ(Skill::classId(), Skill::classId()); }

TEST_F(SkillTest, ClassIdDiffersFromBaseObjectClassId)
{
  EXPECT_NE(Skill::classId(), BaseObject::classId());
}

TEST_F(SkillTest, TypeIdMatchesClassId) { EXPECT_EQ(skill->typeId(), Skill::classId()); }

TEST_F(SkillTest, ObjectIdIsNonNil) { EXPECT_FALSE(skill->objectId().is_nil()); }

// ── Default state ─────────────────────────────────────────────────────────────

TEST_F(SkillTest, DefaultDifficultyIsZero) { EXPECT_EQ(skill->difficulty(), 0); }

TEST_F(SkillTest, DefaultTypeIsMental) { EXPECT_EQ(skill->type(), Skill::SkillType::Mental); }

TEST_F(SkillTest, DefaultAttributeRefIsNull) { EXPECT_EQ(skill->attributeRef(), nullptr); }

// ── difficulty property ───────────────────────────────────────────────────────

TEST_F(SkillTest, SetDifficultyUpdatesDifficulty)
{
  skill->setDifficulty(3);
  EXPECT_EQ(skill->difficulty(), 3);
}

TEST_F(SkillTest, SetDifficultyEmitsDifficultyChanged)
{
  MockSkillObserver observer;
  QObject::connect(skill.get(), &Skill::difficultyChanged,
                   [&observer](int v) { observer.onDifficultyChanged(v); });

  EXPECT_CALL(observer, onDifficultyChanged(3)).Times(1);
  skill->setDifficulty(3);
}

TEST_F(SkillTest, SetDifficultyToSameValueDoesNotEmitSignal)
{
  skill->setDifficulty(3);

  MockSkillObserver observer;
  QObject::connect(skill.get(), &Skill::difficultyChanged,
                   [&observer](int v) { observer.onDifficultyChanged(v); });

  EXPECT_CALL(observer, onDifficultyChanged(::testing::_)).Times(0);
  skill->setDifficulty(3); // same value
}

TEST_F(SkillTest, SetDifficultyToNegativeValue)
{
  skill->setDifficulty(-1);
  EXPECT_EQ(skill->difficulty(), -1);
}

// ── type property ─────────────────────────────────────────────────────────────

TEST_F(SkillTest, SetTypeUpdatesType)
{
  skill->setType(Skill::SkillType::Physical);
  EXPECT_EQ(skill->type(), Skill::SkillType::Physical);
}

TEST_F(SkillTest, SetTypeEmitsTypeChanged)
{
  MockSkillObserver observer;
  QObject::connect(skill.get(), &Skill::typeChanged,
                   [&observer](Skill::SkillType v) { observer.onTypeChanged(v); });

  EXPECT_CALL(observer, onTypeChanged(Skill::SkillType::Physical)).Times(1);
  skill->setType(Skill::SkillType::Physical);
}

TEST_F(SkillTest, SetTypeToSameValueDoesNotEmitSignal)
{
  skill->setType(Skill::SkillType::Physical);

  MockSkillObserver observer;
  QObject::connect(skill.get(), &Skill::typeChanged,
                   [&observer](Skill::SkillType v) { observer.onTypeChanged(v); });

  EXPECT_CALL(observer, onTypeChanged(::testing::_)).Times(0);
  skill->setType(Skill::SkillType::Physical); // same value
}

TEST_F(SkillTest, SetTypeToMentalFromPhysical)
{
  skill->setType(Skill::SkillType::Physical);
  skill->setType(Skill::SkillType::Mental);
  EXPECT_EQ(skill->type(), Skill::SkillType::Mental);
}

// ── attributeRef ──────────────────────────────────────────────────────────────

TEST_F(SkillTest, SetAttributeRefSetsFirstChild)
{
  auto ref = std::make_shared<ParentRef>();
  skill->setAttributeRef(ref.get());
  EXPECT_EQ(skill->attributeRef(), ref.get());
}

TEST_F(SkillTest, SetAttributeRefEmitsAttributeRefChanged)
{
  MockSkillObserver observer;
  QObject::connect(skill.get(), &Skill::attributeRefChanged,
                   [&observer]() { observer.onAttributeRefChanged(); });

  auto ref = std::make_shared<ParentRef>();
  EXPECT_CALL(observer, onAttributeRefChanged()).Times(1);
  skill->setAttributeRef(ref.get());
}

TEST_F(SkillTest, SetAttributeRefReplacesExistingRef)
{
  auto ref1 = std::make_shared<ParentRef>();
  auto ref2 = std::make_shared<ParentRef>();

  skill->setAttributeRef(ref1.get());
  skill->setAttributeRef(ref2.get());

  EXPECT_EQ(skill->attributeRef(), ref2.get());
  EXPECT_EQ(skill->size(), 1); // Only one child
}

TEST_F(SkillTest, AttributeRefAccessibleViaTreeItemInterface)
{
  auto ref = std::make_shared<ParentRef>();
  ref->setTargetIdString("aaaaaaaa-bbbb-4ccc-8ddd-eeeeeeeeeeee");
  skill->setAttributeRef(ref.get());

  // Access via TreeItem interface
  auto* foundRef = dynamic_cast<ParentRef*>(skill->childAt(0).get());
  ASSERT_NE(foundRef, nullptr);
  EXPECT_EQ(foundRef->targetIdString(), "aaaaaaaa-bbbb-4ccc-8ddd-eeeeeeeeeeee");
}

TEST_F(SkillTest, AttributeRefIsFirstChild)
{
  auto ref = std::make_shared<ParentRef>();
  skill->setAttributeRef(ref.get());
  ASSERT_EQ(skill->size(), 1);
  EXPECT_NE(dynamic_cast<ParentRef*>(skill->childAt(0).get()), nullptr);
}

// ── Integration ───────────────────────────────────────────────────────────────

TEST_F(SkillTest, CanSetAllProperties)
{
  skill->setName("Swordsmanship");
  skill->setDescription("The art of fighting with swords");
  skill->setDifficulty(2);
  skill->setType(Skill::SkillType::Physical);

  auto ref = std::make_shared<ParentRef>();
  skill->setAttributeRef(ref.get());

  EXPECT_EQ(skill->name(), "Swordsmanship");
  EXPECT_EQ(skill->description(), "The art of fighting with swords");
  EXPECT_EQ(skill->difficulty(), 2);
  EXPECT_EQ(skill->type(), Skill::SkillType::Physical);
  EXPECT_NE(skill->attributeRef(), nullptr);
}
