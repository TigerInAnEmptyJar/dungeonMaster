#include <secondaryAttribute.hpp>

#include <derivationFormula.hpp>
#include <linearFormula.hpp>
#include <lookupDerivationFormula.hpp>
#include <parentRef.hpp>
#include <quadraticDerivationFormula.hpp>
#include <scaledSumDerivationFormula.hpp>

#include <cpTable.hpp>

#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>

// ── Qt event loop ─────────────────────────────────────────────────────────────
// QCoreApplication is provided by baseObjectTest.cpp (same test executable).

using namespace gurps_system;

// ── Identity ──────────────────────────────────────────────────────────────────

TEST(SecondaryAttributeTest, ClassIdIsNonNil)
{
  EXPECT_FALSE(SecondaryAttribute::classId().is_nil());
}

TEST(SecondaryAttributeTest, ClassIdIsStable)
{
  EXPECT_EQ(SecondaryAttribute::classId(), SecondaryAttribute::classId());
}

TEST(SecondaryAttributeTest, TypeIdMatchesClassId)
{
  auto sa = std::make_shared<SecondaryAttribute>();
  EXPECT_EQ(sa->typeId(), SecondaryAttribute::classId());
}

TEST(SecondaryAttributeTest, ObjectIdIsNonNil)
{
  auto sa = std::make_shared<SecondaryAttribute>();
  EXPECT_FALSE(sa->objectId().is_nil());
}

// ── Default state ─────────────────────────────────────────────────────────────

TEST(SecondaryAttributeTest, DefaultMaxDirectBonusIsMinusOne)
{
  auto sa = std::make_shared<SecondaryAttribute>();
  EXPECT_EQ(sa->maxDirectBonus(), -1);
}

TEST(SecondaryAttributeTest, DefaultDerivationFormulaIsNullptr)
{
  auto sa = std::make_shared<SecondaryAttribute>();
  EXPECT_EQ(sa->derivationFormula(), nullptr);
}

TEST(SecondaryAttributeTest, DefaultParentCountIsZero)
{
  auto sa = std::make_shared<SecondaryAttribute>();
  EXPECT_EQ(sa->parentCount(), 0);
}

TEST(SecondaryAttributeTest, DefaultHasDirectFormulaIsFalse)
{
  auto sa = std::make_shared<SecondaryAttribute>();
  EXPECT_FALSE(sa->hasDirectFormula());
}

TEST(SecondaryAttributeTest, DirectFormulaThrowsWhenAbsent)
{
  auto sa = std::make_shared<SecondaryAttribute>();
  EXPECT_THROW(sa->directFormula(), std::logic_error);
}

// ── maxDirectBonus property ───────────────────────────────────────────────────

TEST(SecondaryAttributeTest, SetMaxDirectBonusUpdatesValue)
{
  auto sa = std::make_shared<SecondaryAttribute>();
  sa->setMaxDirectBonus(5);
  EXPECT_EQ(sa->maxDirectBonus(), 5);
}

// ── insertDerivationFormula ───────────────────────────────────────────────────

TEST(SecondaryAttributeTest, InsertDerivationFormulaSucceeds)
{
  auto sa = std::make_shared<SecondaryAttribute>();
  auto df = std::make_shared<ScaledSumDerivationFormula>(QList<int>{1, 1}, 4);
  EXPECT_NO_THROW(sa->insertDerivationFormula(df));
  EXPECT_NE(sa->derivationFormula(), nullptr);
}

TEST(SecondaryAttributeTest, InsertDerivationFormulaPlacedAtChildZero)
{
  auto sa = std::make_shared<SecondaryAttribute>();
  auto df = std::make_shared<ScaledSumDerivationFormula>(QList<int>{1, 1}, 4);
  sa->insertDerivationFormula(df);
  EXPECT_EQ(sa->childAt(0), df);
}

TEST(SecondaryAttributeTest, InsertDerivationFormulaTwiceThrows)
{
  auto sa = std::make_shared<SecondaryAttribute>();
  sa->insertDerivationFormula(std::make_shared<QuadraticDerivationFormula>(5));
  EXPECT_THROW(sa->insertDerivationFormula(std::make_shared<QuadraticDerivationFormula>(5)),
               std::logic_error);
}

// ── addParent ─────────────────────────────────────────────────────────────────

TEST(SecondaryAttributeTest, AddParentIncreasesParentCount)
{
  auto sa = std::make_shared<SecondaryAttribute>();
  sa->addParent(std::make_shared<ParentRef>());
  EXPECT_EQ(sa->parentCount(), 1);
}

TEST(SecondaryAttributeTest, AddMultipleParentsIncreasesParentCount)
{
  auto sa = std::make_shared<SecondaryAttribute>();
  sa->addParent(std::make_shared<ParentRef>());
  sa->addParent(std::make_shared<ParentRef>());
  EXPECT_EQ(sa->parentCount(), 2);
}

TEST(SecondaryAttributeTest, ParentAtReturnsCorrectRef)
{
  auto sa = std::make_shared<SecondaryAttribute>();
  auto pr = std::make_shared<ParentRef>();
  sa->addParent(pr);
  EXPECT_EQ(&sa->parentAt(0), pr.get());
}

TEST(SecondaryAttributeTest, ParentAtOutOfRangeThrows)
{
  auto sa = std::make_shared<SecondaryAttribute>();
  EXPECT_THROW(sa->parentAt(0), std::out_of_range);
}

// ── insertDirectFormula ───────────────────────────────────────────────────────

TEST(SecondaryAttributeTest, InsertDirectFormulaSucceeds)
{
  auto sa = std::make_shared<SecondaryAttribute>();
  EXPECT_NO_THROW(sa->insertDirectFormula(std::make_shared<LinearFormula>(10)));
  EXPECT_TRUE(sa->hasDirectFormula());
}

TEST(SecondaryAttributeTest, InsertDirectFormulaTwiceThrows)
{
  auto sa = std::make_shared<SecondaryAttribute>();
  sa->insertDirectFormula(std::make_shared<LinearFormula>(10));
  EXPECT_THROW(sa->insertDirectFormula(std::make_shared<LinearFormula>(5)), std::logic_error);
}

TEST(SecondaryAttributeTest, DirectFormulaIsAccessibleAfterInsert)
{
  auto sa = std::make_shared<SecondaryAttribute>();
  auto f = std::make_shared<LinearFormula>(10);
  sa->insertDirectFormula(f);
  EXPECT_EQ(&sa->directFormula(), f.get());
}

// ── Child layout ordering ─────────────────────────────────────────────────────

TEST(SecondaryAttributeTest, ParentIsInsertedBeforeDirectFormula)
{
  // child layout: [Formula, then ParentRef] should end up as [ParentRef, Formula]
  auto sa = std::make_shared<SecondaryAttribute>();
  auto f = std::make_shared<LinearFormula>(10);
  auto pr = std::make_shared<ParentRef>();

  sa->insertDirectFormula(f); // formula appended first → child[0]
  sa->addParent(pr);          // parent should go before formula → child[0]

  EXPECT_EQ(sa->childAt(0), pr); // ParentRef before Formula
  EXPECT_EQ(sa->childAt(1), f);
}

TEST(SecondaryAttributeTest, DerivationFormulaPreceedsParentAndDirectFormula)
{
  auto sa = std::make_shared<SecondaryAttribute>();
  auto df = std::make_shared<QuadraticDerivationFormula>(5);
  auto pr = std::make_shared<ParentRef>();
  auto f = std::make_shared<LinearFormula>(10);

  sa->insertDerivationFormula(df);
  sa->addParent(pr);
  sa->insertDirectFormula(f);

  // expected layout: [df, pr, f]
  EXPECT_EQ(sa->childAt(0), df);
  EXPECT_EQ(sa->childAt(1), pr);
  EXPECT_EQ(sa->childAt(2), f);
  EXPECT_EQ(sa->size(), 3);
}

TEST(SecondaryAttributeTest, LookupFormulaDescriptorIsUsable)
{
  auto sa = std::make_shared<SecondaryAttribute>();
  auto df = std::make_shared<LookupDerivationFormula>(CpTable{{1, 7}, {2, 14}});
  sa->insertDerivationFormula(df);
  EXPECT_EQ(sa->derivationFormula(), df.get());
}
