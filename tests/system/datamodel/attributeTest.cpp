#include <attribute.hpp>
#include <baseObject.hpp>
#include <derivationFormula.hpp>
#include <formula.hpp>
#include <linearFormula.hpp>
#include <parentRef.hpp>
#include <scaledSumDerivationFormula.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

// ── Qt event loop ─────────────────────────────────────────────────────────────
// QCoreApplication is provided by baseObjectTest.cpp (same test executable).

using namespace gurps_system;

// ── Helpers ───────────────────────────────────────────────────────────────────

static auto makeAttrWithFormula() -> std::shared_ptr<Attribute>
{
  auto attr = std::make_shared<Attribute>();
  attr->insertDirectFormula(std::make_shared<LinearFormula>(10));
  return attr;
}

// ── Fixture ───────────────────────────────────────────────────────────────────

class AttributeTest : public ::testing::Test
{
protected:
  std::shared_ptr<Attribute> attr{makeAttrWithFormula()};
};

// ── Identity ──────────────────────────────────────────────────────────────────

TEST_F(AttributeTest, ClassIdIsNonNil) { EXPECT_FALSE(Attribute::classId().is_nil()); }

TEST_F(AttributeTest, ClassIdIsStable) { EXPECT_EQ(Attribute::classId(), Attribute::classId()); }

TEST_F(AttributeTest, ClassIdDiffersFromBaseObjectClassId)
{
  EXPECT_NE(Attribute::classId(), BaseObject::classId());
}

TEST_F(AttributeTest, TypeIdMatchesClassId) { EXPECT_EQ(attr->typeId(), Attribute::classId()); }

TEST_F(AttributeTest, ObjectIdIsNonNil) { EXPECT_FALSE(attr->objectId().is_nil()); }

// ── Creation ──────────────────────────────────────────────────────────────────

TEST_F(AttributeTest, CreateWithoutFormulaSucceeds)
{
  EXPECT_NO_THROW(std::make_shared<Attribute>());
}

// ── Direct formula ────────────────────────────────────────────────────────────

TEST_F(AttributeTest, HasDirectFormulaFalseOnFreshAttribute)
{
  auto a = std::make_shared<Attribute>();
  EXPECT_FALSE(a->hasDirectFormula());
}

TEST_F(AttributeTest, HasDirectFormulaTrueAfterInsert) { EXPECT_TRUE(attr->hasDirectFormula()); }

TEST_F(AttributeTest, InsertDirectFormulaAddsOneChild)
{
  auto a = std::make_shared<Attribute>();
  a->insertDirectFormula(std::make_shared<LinearFormula>(10));
  EXPECT_EQ(a->size(), 1);
}

TEST_F(AttributeTest, InsertDirectFormulaTwiceThrows)
{
  EXPECT_THROW(attr->insertDirectFormula(std::make_shared<LinearFormula>(5)), std::logic_error);
}

TEST_F(AttributeTest, DirectFormulaThrowsWhenAbsent)
{
  auto a = std::make_shared<Attribute>();
  EXPECT_THROW(std::ignore = a->directFormula(), std::logic_error);
}

TEST_F(AttributeTest, DirectFormulaReturnsInsertedFormula)
{
  EXPECT_NE(dynamic_cast<LinearFormula const*>(&attr->directFormula()), nullptr);
}

TEST_F(AttributeTest, FormulaIsFirstChild) { EXPECT_NE(attr->childAt(0), nullptr); }

TEST_F(AttributeTest, FirstChildIsAFormula)
{
  EXPECT_NE(dynamic_cast<Formula*>(attr->childAt(0).get()), nullptr);
}

// ── Derivation formula ────────────────────────────────────────────────────────

TEST_F(AttributeTest, DerivationFormulaIsNullptrOnFreshAttribute)
{
  auto a = std::make_shared<Attribute>();
  EXPECT_EQ(a->derivationFormula(), nullptr);
}

TEST_F(AttributeTest, InsertDerivationFormulaSucceeds)
{
  auto a = std::make_shared<Attribute>();
  EXPECT_NO_THROW(
      a->insertDerivationFormula(std::make_shared<ScaledSumDerivationFormula>(QList<int>{1}, 1)));
}

TEST_F(AttributeTest, InsertDerivationFormulaPlacesItAtIndexZero)
{
  auto a = std::make_shared<Attribute>();
  a->insertDerivationFormula(std::make_shared<ScaledSumDerivationFormula>(QList<int>{1}, 1));
  EXPECT_NE(dynamic_cast<DerivationFormula*>(a->childAt(0).get()), nullptr);
}

TEST_F(AttributeTest, InsertDerivationFormulaTwiceThrows)
{
  auto a = std::make_shared<Attribute>();
  a->insertDerivationFormula(std::make_shared<ScaledSumDerivationFormula>(QList<int>{1}, 1));
  EXPECT_THROW(
      a->insertDerivationFormula(std::make_shared<ScaledSumDerivationFormula>(QList<int>{1}, 1)),
      std::logic_error);
}

TEST_F(AttributeTest, DerivationFormulaReturnsInsertedFormula)
{
  auto a = std::make_shared<Attribute>();
  a->insertDerivationFormula(std::make_shared<ScaledSumDerivationFormula>(QList<int>{1}, 1));
  EXPECT_NE(dynamic_cast<ScaledSumDerivationFormula*>(a->derivationFormula()), nullptr);
}

TEST_F(AttributeTest, DerivationFormulaRemainsAtIndexZeroAfterDirectFormulaInserted)
{
  auto a = std::make_shared<Attribute>();
  a->insertDerivationFormula(std::make_shared<ScaledSumDerivationFormula>(QList<int>{1}, 1));
  a->insertDirectFormula(std::make_shared<LinearFormula>(5));
  EXPECT_NE(dynamic_cast<DerivationFormula*>(a->childAt(0).get()), nullptr);
}

// ── Parent references ─────────────────────────────────────────────────────────

TEST_F(AttributeTest, ParentCountIsZeroOnFreshAttribute)
{
  EXPECT_EQ(std::make_shared<Attribute>()->parentCount(), 0);
}

TEST_F(AttributeTest, AddParentIncreasesParentCount)
{
  auto a = std::make_shared<Attribute>();
  a->addParent(std::make_shared<ParentRef>());
  EXPECT_EQ(a->parentCount(), 1);
}

TEST_F(AttributeTest, AddTwoParentsGivesParentCountTwo)
{
  auto a = std::make_shared<Attribute>();
  a->addParent(std::make_shared<ParentRef>());
  a->addParent(std::make_shared<ParentRef>());
  EXPECT_EQ(a->parentCount(), 2);
}

TEST_F(AttributeTest, ParentAtZeroReturnsInsertedParent)
{
  auto a = std::make_shared<Attribute>();
  auto ref = std::make_shared<ParentRef>();
  ref->setTargetIdString("aaaaaaaa-bbbb-4ccc-8ddd-eeeeeeeeeeee");
  a->addParent(ref);
  EXPECT_EQ(a->parentAt(0).targetIdString(), "aaaaaaaa-bbbb-4ccc-8ddd-eeeeeeeeeeee");
}

TEST_F(AttributeTest, ParentAtOutOfRangeThrows)
{
  auto a = std::make_shared<Attribute>();
  EXPECT_THROW(std::ignore = a->parentAt(0), std::out_of_range);
}

TEST_F(AttributeTest, ParentInsertedBeforeDirectFormula)
{
  // Layout: [DerivationFormula, ParentRef, DirectFormula]
  auto a = std::make_shared<Attribute>();
  a->insertDerivationFormula(std::make_shared<ScaledSumDerivationFormula>(QList<int>{1}, 1));
  a->insertDirectFormula(std::make_shared<LinearFormula>(5));
  a->addParent(std::make_shared<ParentRef>());
  ASSERT_EQ(a->size(), 3);
  EXPECT_NE(dynamic_cast<DerivationFormula*>(a->childAt(0).get()), nullptr);
  EXPECT_NE(dynamic_cast<ParentRef*>(a->childAt(1).get()), nullptr);
  EXPECT_NE(dynamic_cast<Formula*>(a->childAt(2).get()), nullptr);
}

// ── BaseObject / TreeItem integration ─────────────────────────────────────────

TEST_F(AttributeTest, IsABaseObject)
{
  EXPECT_NE(dynamic_cast<gurps_system::BaseObject*>(attr.get()), nullptr);
}

TEST_F(AttributeTest, IsATreeItem)
{
  EXPECT_NE(dynamic_cast<infrastructure::TreeItem*>(attr.get()), nullptr);
}

TEST_F(AttributeTest, InheritsNameProperty)
{
  attr->setName(QStringLiteral("Strength"));
  EXPECT_EQ(attr->name(), QStringLiteral("Strength"));
}
