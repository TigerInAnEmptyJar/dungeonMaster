#include "prerequisite.hpp"
#include "attributeSkillPrerequisite.hpp"
#include "compositePrerequisites.hpp"
#include "simplePrerequisites.hpp"

#include "parentRef.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

// ── Signal observers ──────────────────────────────────────────────────────────

namespace {

struct MockAttributePrerequisiteObserver
{
  MOCK_METHOD(void, onMinimumValueChanged, (int));
  MOCK_METHOD(void, onAttributeRefChanged, ());
};

struct MockSkillPrerequisiteObserver
{
  MOCK_METHOD(void, onMinimumValueChanged, (int));
  MOCK_METHOD(void, onSkillRefChanged, ());
};

struct MockAdvantagePrerequisiteObserver
{
  MOCK_METHOD(void, onAdvantageRefChanged, ());
};

struct MockProfessionPrerequisiteObserver
{
  MOCK_METHOD(void, onProfessionRefChanged, ());
};

struct MockRacePrerequisiteObserver
{
  MOCK_METHOD(void, onRaceRefChanged, ());
};

} // namespace

using namespace gurps_system;

// ── AttributePrerequisite Tests ───────────────────────────────────────────────

class AttributePrerequisiteTest : public ::testing::Test
{
protected:
  std::shared_ptr<AttributePrerequisite> prerequisite;

  void SetUp() override { prerequisite = std::make_shared<AttributePrerequisite>(); }
};

TEST_F(AttributePrerequisiteTest, ClassIdIsNonNil)
{
  EXPECT_FALSE(AttributePrerequisite::classId().is_nil());
}

TEST_F(AttributePrerequisiteTest, ClassIdIsStable)
{
  EXPECT_EQ(AttributePrerequisite::classId(), AttributePrerequisite::classId());
}

TEST_F(AttributePrerequisiteTest, TypeIdMatchesClassId)
{
  EXPECT_EQ(prerequisite->typeId(), AttributePrerequisite::classId());
}

TEST_F(AttributePrerequisiteTest, ObjectIdIsNonNil)
{
  EXPECT_FALSE(prerequisite->objectId().is_nil());
}

TEST_F(AttributePrerequisiteTest, DefaultMinimumValueIsZero)
{
  EXPECT_EQ(prerequisite->minimumValue(), 0);
}

TEST_F(AttributePrerequisiteTest, DefaultAttributeRefIsNull)
{
  EXPECT_EQ(prerequisite->attributeRef(), nullptr);
}

TEST_F(AttributePrerequisiteTest, SetMinimumValueUpdatesValue)
{
  prerequisite->setMinimumValue(12);
  EXPECT_EQ(prerequisite->minimumValue(), 12);
}

TEST_F(AttributePrerequisiteTest, SetMinimumValueEmitsSignal)
{
  MockAttributePrerequisiteObserver observer;
  QObject::connect(prerequisite.get(), &AttributePrerequisite::minimumValueChanged,
                   [&observer](int v) { observer.onMinimumValueChanged(v); });

  EXPECT_CALL(observer, onMinimumValueChanged(12)).Times(1);
  prerequisite->setMinimumValue(12);
}

TEST_F(AttributePrerequisiteTest, SetMinimumValueToSameValueDoesNotEmitSignal)
{
  prerequisite->setMinimumValue(12);

  MockAttributePrerequisiteObserver observer;
  QObject::connect(prerequisite.get(), &AttributePrerequisite::minimumValueChanged,
                   [&observer](int v) { observer.onMinimumValueChanged(v); });

  EXPECT_CALL(observer, onMinimumValueChanged(::testing::_)).Times(0);
  prerequisite->setMinimumValue(12);
}

TEST_F(AttributePrerequisiteTest, SetAttributeRefSetsFirstChild)
{
  auto ref = std::make_shared<ParentRef>();
  prerequisite->setAttributeRef(ref.get());

  EXPECT_EQ(prerequisite->size(), 1);
  EXPECT_EQ(prerequisite->childAt(0), ref);
}

TEST_F(AttributePrerequisiteTest, SetAttributeRefEmitsSignal)
{
  auto ref = std::make_shared<ParentRef>();

  MockAttributePrerequisiteObserver observer;
  QObject::connect(prerequisite.get(), &AttributePrerequisite::attributeRefChanged,
                   [&observer]() { observer.onAttributeRefChanged(); });

  EXPECT_CALL(observer, onAttributeRefChanged()).Times(1);
  prerequisite->setAttributeRef(ref.get());
}

TEST_F(AttributePrerequisiteTest, SetAttributeRefReplacesExistingRef)
{
  auto ref1 = std::make_shared<ParentRef>();
  auto ref2 = std::make_shared<ParentRef>();

  prerequisite->setAttributeRef(ref1.get());
  prerequisite->setAttributeRef(ref2.get());

  EXPECT_EQ(prerequisite->size(), 1);
  EXPECT_EQ(prerequisite->childAt(0), ref2);
}

TEST_F(AttributePrerequisiteTest, AttributeRefReturnsFirstChild)
{
  auto ref = std::make_shared<ParentRef>();
  prerequisite->setAttributeRef(ref.get());

  EXPECT_EQ(prerequisite->attributeRef(), ref.get());
}

// ── SkillPrerequisite Tests ───────────────────────────────────────────────────

class SkillPrerequisiteTest : public ::testing::Test
{
protected:
  std::shared_ptr<SkillPrerequisite> prerequisite;

  void SetUp() override { prerequisite = std::make_shared<SkillPrerequisite>(); }
};

TEST_F(SkillPrerequisiteTest, ClassIdIsNonNil)
{
  EXPECT_FALSE(SkillPrerequisite::classId().is_nil());
}

TEST_F(SkillPrerequisiteTest, ClassIdIsStable)
{
  EXPECT_EQ(SkillPrerequisite::classId(), SkillPrerequisite::classId());
}

TEST_F(SkillPrerequisiteTest, TypeIdMatchesClassId)
{
  EXPECT_EQ(prerequisite->typeId(), SkillPrerequisite::classId());
}

TEST_F(SkillPrerequisiteTest, ObjectIdIsNonNil) { EXPECT_FALSE(prerequisite->objectId().is_nil()); }

TEST_F(SkillPrerequisiteTest, DefaultMinimumValueIsZero)
{
  EXPECT_EQ(prerequisite->minimumValue(), 0);
}

TEST_F(SkillPrerequisiteTest, DefaultSkillRefIsNull)
{
  EXPECT_EQ(prerequisite->skillRef(), nullptr);
}

TEST_F(SkillPrerequisiteTest, SetMinimumValueUpdatesValue)
{
  prerequisite->setMinimumValue(15);
  EXPECT_EQ(prerequisite->minimumValue(), 15);
}

TEST_F(SkillPrerequisiteTest, SetMinimumValueEmitsSignal)
{
  MockSkillPrerequisiteObserver observer;
  QObject::connect(prerequisite.get(), &SkillPrerequisite::minimumValueChanged,
                   [&observer](int v) { observer.onMinimumValueChanged(v); });

  EXPECT_CALL(observer, onMinimumValueChanged(15)).Times(1);
  prerequisite->setMinimumValue(15);
}

TEST_F(SkillPrerequisiteTest, SetSkillRefSetsFirstChild)
{
  auto ref = std::make_shared<ParentRef>();
  prerequisite->setSkillRef(ref.get());

  EXPECT_EQ(prerequisite->size(), 1);
  EXPECT_EQ(prerequisite->childAt(0), ref);
}

TEST_F(SkillPrerequisiteTest, SetSkillRefEmitsSignal)
{
  auto ref = std::make_shared<ParentRef>();

  MockSkillPrerequisiteObserver observer;
  QObject::connect(prerequisite.get(), &SkillPrerequisite::skillRefChanged,
                   [&observer]() { observer.onSkillRefChanged(); });

  EXPECT_CALL(observer, onSkillRefChanged()).Times(1);
  prerequisite->setSkillRef(ref.get());
}

TEST_F(SkillPrerequisiteTest, SkillRefReturnsFirstChild)
{
  auto ref = std::make_shared<ParentRef>();
  prerequisite->setSkillRef(ref.get());

  EXPECT_EQ(prerequisite->skillRef(), ref.get());
}

// ── AdvantagePrerequisite Tests ───────────────────────────────────────────────

class AdvantagePrerequisiteTest : public ::testing::Test
{
protected:
  std::shared_ptr<AdvantagePrerequisite> prerequisite;

  void SetUp() override { prerequisite = std::make_shared<AdvantagePrerequisite>(); }
};

TEST_F(AdvantagePrerequisiteTest, ClassIdIsNonNil)
{
  EXPECT_FALSE(AdvantagePrerequisite::classId().is_nil());
}

TEST_F(AdvantagePrerequisiteTest, ClassIdIsStable)
{
  EXPECT_EQ(AdvantagePrerequisite::classId(), AdvantagePrerequisite::classId());
}

TEST_F(AdvantagePrerequisiteTest, TypeIdMatchesClassId)
{
  EXPECT_EQ(prerequisite->typeId(), AdvantagePrerequisite::classId());
}

TEST_F(AdvantagePrerequisiteTest, ObjectIdIsNonNil)
{
  EXPECT_FALSE(prerequisite->objectId().is_nil());
}

TEST_F(AdvantagePrerequisiteTest, DefaultAdvantageRefIsNull)
{
  EXPECT_EQ(prerequisite->advantageRef(), nullptr);
}

TEST_F(AdvantagePrerequisiteTest, SetAdvantageRefSetsFirstChild)
{
  auto ref = std::make_shared<ParentRef>();
  prerequisite->setAdvantageRef(ref.get());

  EXPECT_EQ(prerequisite->size(), 1);
  EXPECT_EQ(prerequisite->childAt(0), ref);
}

TEST_F(AdvantagePrerequisiteTest, SetAdvantageRefEmitsSignal)
{
  auto ref = std::make_shared<ParentRef>();

  MockAdvantagePrerequisiteObserver observer;
  QObject::connect(prerequisite.get(), &AdvantagePrerequisite::advantageRefChanged,
                   [&observer]() { observer.onAdvantageRefChanged(); });

  EXPECT_CALL(observer, onAdvantageRefChanged()).Times(1);
  prerequisite->setAdvantageRef(ref.get());
}

TEST_F(AdvantagePrerequisiteTest, AdvantageRefReturnsFirstChild)
{
  auto ref = std::make_shared<ParentRef>();
  prerequisite->setAdvantageRef(ref.get());

  EXPECT_EQ(prerequisite->advantageRef(), ref.get());
}

// ── ProfessionPrerequisite Tests ──────────────────────────────────────────────

class ProfessionPrerequisiteTest : public ::testing::Test
{
protected:
  std::shared_ptr<ProfessionPrerequisite> prerequisite;

  void SetUp() override { prerequisite = std::make_shared<ProfessionPrerequisite>(); }
};

TEST_F(ProfessionPrerequisiteTest, ClassIdIsNonNil)
{
  EXPECT_FALSE(ProfessionPrerequisite::classId().is_nil());
}

TEST_F(ProfessionPrerequisiteTest, ClassIdIsStable)
{
  EXPECT_EQ(ProfessionPrerequisite::classId(), ProfessionPrerequisite::classId());
}

TEST_F(ProfessionPrerequisiteTest, TypeIdMatchesClassId)
{
  EXPECT_EQ(prerequisite->typeId(), ProfessionPrerequisite::classId());
}

TEST_F(ProfessionPrerequisiteTest, ObjectIdIsNonNil)
{
  EXPECT_FALSE(prerequisite->objectId().is_nil());
}

TEST_F(ProfessionPrerequisiteTest, DefaultProfessionRefIsNull)
{
  EXPECT_EQ(prerequisite->professionRef(), nullptr);
}

TEST_F(ProfessionPrerequisiteTest, SetProfessionRefSetsFirstChild)
{
  auto ref = std::make_shared<ParentRef>();
  prerequisite->setProfessionRef(ref.get());

  EXPECT_EQ(prerequisite->size(), 1);
  EXPECT_EQ(prerequisite->childAt(0), ref);
}

TEST_F(ProfessionPrerequisiteTest, SetProfessionRefEmitsSignal)
{
  auto ref = std::make_shared<ParentRef>();

  MockProfessionPrerequisiteObserver observer;
  QObject::connect(prerequisite.get(), &ProfessionPrerequisite::professionRefChanged,
                   [&observer]() { observer.onProfessionRefChanged(); });

  EXPECT_CALL(observer, onProfessionRefChanged()).Times(1);
  prerequisite->setProfessionRef(ref.get());
}

TEST_F(ProfessionPrerequisiteTest, ProfessionRefReturnsFirstChild)
{
  auto ref = std::make_shared<ParentRef>();
  prerequisite->setProfessionRef(ref.get());

  EXPECT_EQ(prerequisite->professionRef(), ref.get());
}

// ── RacePrerequisite Tests ────────────────────────────────────────────────────

class RacePrerequisiteTest : public ::testing::Test
{
protected:
  std::shared_ptr<RacePrerequisite> prerequisite;

  void SetUp() override { prerequisite = std::make_shared<RacePrerequisite>(); }
};

TEST_F(RacePrerequisiteTest, ClassIdIsNonNil)
{
  EXPECT_FALSE(RacePrerequisite::classId().is_nil());
}

TEST_F(RacePrerequisiteTest, ClassIdIsStable)
{
  EXPECT_EQ(RacePrerequisite::classId(), RacePrerequisite::classId());
}

TEST_F(RacePrerequisiteTest, TypeIdMatchesClassId)
{
  EXPECT_EQ(prerequisite->typeId(), RacePrerequisite::classId());
}

TEST_F(RacePrerequisiteTest, ObjectIdIsNonNil) { EXPECT_FALSE(prerequisite->objectId().is_nil()); }

TEST_F(RacePrerequisiteTest, DefaultRaceRefIsNull) { EXPECT_EQ(prerequisite->raceRef(), nullptr); }

TEST_F(RacePrerequisiteTest, SetRaceRefSetsFirstChild)
{
  auto ref = std::make_shared<ParentRef>();
  prerequisite->setRaceRef(ref.get());

  EXPECT_EQ(prerequisite->size(), 1);
  EXPECT_EQ(prerequisite->childAt(0), ref);
}

TEST_F(RacePrerequisiteTest, SetRaceRefEmitsSignal)
{
  auto ref = std::make_shared<ParentRef>();

  MockRacePrerequisiteObserver observer;
  QObject::connect(prerequisite.get(), &RacePrerequisite::raceRefChanged,
                   [&observer]() { observer.onRaceRefChanged(); });

  EXPECT_CALL(observer, onRaceRefChanged()).Times(1);
  prerequisite->setRaceRef(ref.get());
}

TEST_F(RacePrerequisiteTest, RaceRefReturnsFirstChild)
{
  auto ref = std::make_shared<ParentRef>();
  prerequisite->setRaceRef(ref.get());

  EXPECT_EQ(prerequisite->raceRef(), ref.get());
}

// ── AndPrerequisite Tests ─────────────────────────────────────────────────────

class AndPrerequisiteTest : public ::testing::Test
{
protected:
  std::shared_ptr<AndPrerequisite> prerequisite;

  void SetUp() override { prerequisite = std::make_shared<AndPrerequisite>(); }
};

TEST_F(AndPrerequisiteTest, ClassIdIsNonNil) { EXPECT_FALSE(AndPrerequisite::classId().is_nil()); }

TEST_F(AndPrerequisiteTest, ClassIdIsStable)
{
  EXPECT_EQ(AndPrerequisite::classId(), AndPrerequisite::classId());
}

TEST_F(AndPrerequisiteTest, TypeIdMatchesClassId)
{
  EXPECT_EQ(prerequisite->typeId(), AndPrerequisite::classId());
}

TEST_F(AndPrerequisiteTest, ObjectIdIsNonNil) { EXPECT_FALSE(prerequisite->objectId().is_nil()); }

TEST_F(AndPrerequisiteTest, StartsWithNoChildren) { EXPECT_EQ(prerequisite->size(), 0); }

TEST_F(AndPrerequisiteTest, CanAddMultiplePrerequisites)
{
  auto child1 = std::make_shared<AttributePrerequisite>();
  auto child2 = std::make_shared<SkillPrerequisite>();

  prerequisite->insertChild(prerequisite->size(), child1);
  prerequisite->insertChild(prerequisite->size(), child2);

  EXPECT_EQ(prerequisite->size(), 2);
  EXPECT_EQ(prerequisite->childAt(0), child1);
  EXPECT_EQ(prerequisite->childAt(1), child2);
}

TEST_F(AndPrerequisiteTest, CanAddDifferentPrerequisiteTypes)
{
  prerequisite->insertChild(prerequisite->size(), std::make_shared<AttributePrerequisite>());
  prerequisite->insertChild(prerequisite->size(), std::make_shared<SkillPrerequisite>());
  prerequisite->insertChild(prerequisite->size(), std::make_shared<AdvantagePrerequisite>());

  EXPECT_EQ(prerequisite->size(), 3);
}

TEST_F(AndPrerequisiteTest, CanNestCompositePrerequisites)
{
  auto nestedAnd = std::make_shared<AndPrerequisite>();
  nestedAnd->insertChild(nestedAnd->size(), std::make_shared<AttributePrerequisite>());

  prerequisite->insertChild(prerequisite->size(), nestedAnd);
  prerequisite->insertChild(prerequisite->size(), std::make_shared<SkillPrerequisite>());

  EXPECT_EQ(prerequisite->size(), 2);
  EXPECT_EQ(prerequisite->childAt(0), nestedAnd);
}

// ── OrPrerequisite Tests ──────────────────────────────────────────────────────

class OrPrerequisiteTest : public ::testing::Test
{
protected:
  std::shared_ptr<OrPrerequisite> prerequisite;

  void SetUp() override { prerequisite = std::make_shared<OrPrerequisite>(); }
};

TEST_F(OrPrerequisiteTest, ClassIdIsNonNil) { EXPECT_FALSE(OrPrerequisite::classId().is_nil()); }

TEST_F(OrPrerequisiteTest, ClassIdIsStable)
{
  EXPECT_EQ(OrPrerequisite::classId(), OrPrerequisite::classId());
}

TEST_F(OrPrerequisiteTest, TypeIdMatchesClassId)
{
  EXPECT_EQ(prerequisite->typeId(), OrPrerequisite::classId());
}

TEST_F(OrPrerequisiteTest, ObjectIdIsNonNil) { EXPECT_FALSE(prerequisite->objectId().is_nil()); }

TEST_F(OrPrerequisiteTest, StartsWithNoChildren) { EXPECT_EQ(prerequisite->size(), 0); }

TEST_F(OrPrerequisiteTest, CanAddMultiplePrerequisites)
{
  auto child1 = std::make_shared<ProfessionPrerequisite>();
  auto child2 = std::make_shared<RacePrerequisite>();

  prerequisite->insertChild(prerequisite->size(), child1);
  prerequisite->insertChild(prerequisite->size(), child2);

  EXPECT_EQ(prerequisite->size(), 2);
  EXPECT_EQ(prerequisite->childAt(0), child1);
  EXPECT_EQ(prerequisite->childAt(1), child2);
}

TEST_F(OrPrerequisiteTest, CanAddDifferentPrerequisiteTypes)
{
  prerequisite->insertChild(prerequisite->size(), std::make_shared<ProfessionPrerequisite>());
  prerequisite->insertChild(prerequisite->size(), std::make_shared<RacePrerequisite>());
  prerequisite->insertChild(prerequisite->size(), std::make_shared<AdvantagePrerequisite>());

  EXPECT_EQ(prerequisite->size(), 3);
}

TEST_F(OrPrerequisiteTest, CanNestCompositePrerequisites)
{
  auto nestedOr = std::make_shared<OrPrerequisite>();
  nestedOr->insertChild(nestedOr->size(), std::make_shared<RacePrerequisite>());

  prerequisite->insertChild(prerequisite->size(), nestedOr);
  prerequisite->insertChild(prerequisite->size(), std::make_shared<ProfessionPrerequisite>());

  EXPECT_EQ(prerequisite->size(), 2);
  EXPECT_EQ(prerequisite->childAt(0), nestedOr);
}

TEST_F(OrPrerequisiteTest, CanMixAndOrPrerequisites)
{
  auto andPrereq = std::make_shared<AndPrerequisite>();
  andPrereq->insertChild(andPrereq->size(), std::make_shared<AttributePrerequisite>());
  andPrereq->insertChild(andPrereq->size(), std::make_shared<SkillPrerequisite>());

  prerequisite->insertChild(prerequisite->size(), andPrereq);
  prerequisite->insertChild(prerequisite->size(), std::make_shared<RacePrerequisite>());

  EXPECT_EQ(prerequisite->size(), 2);
}
