#include <attribute.hpp>
#include <baseObject.hpp>
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
  attr->insertChild(attr->size(), std::make_shared<LinearFormula>(10));
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

TEST_F(AttributeTest, FormulaObjectIsNullOnFreshAttribute)
{
  auto a = std::make_shared<Attribute>();
  EXPECT_EQ(a->formula(), nullptr);
}

TEST_F(AttributeTest, FormulaObjectReturnsFormulaAfterInsert)
{
  EXPECT_NE(attr->formula(), nullptr);
}

TEST_F(AttributeTest, InsertDirectFormulaAddsOneChild)
{
  auto a = std::make_shared<Attribute>();
  a->insertChild(a->size(), std::make_shared<LinearFormula>(10));
  EXPECT_EQ(a->size(), 1);
}

TEST_F(AttributeTest, InsertDirectFormulaTwiceDoesNotThrow)
{
  // With direct TreeItem API, duplicate checks are responsibility of caller
  EXPECT_NO_THROW(attr->insertChild(attr->size(), std::make_shared<LinearFormula>(5)));
}

TEST_F(AttributeTest, FormulaObjectReturnsInsertedFormula)
{
  EXPECT_NE(dynamic_cast<LinearFormula*>(attr->formula()), nullptr);
}

TEST_F(AttributeTest, FormulaIsFirstChild) { EXPECT_NE(attr->childAt(0), nullptr); }

TEST_F(AttributeTest, FirstChildIsAFormula)
{
  EXPECT_NE(dynamic_cast<Formula*>(attr->childAt(0).get()), nullptr);
}

// ── Derivation formula ────────────────────────────────────────────────────────

TEST_F(AttributeTest, FormulaObjectIsNullptrOnFreshAttribute)
{
  auto a = std::make_shared<Attribute>();
  EXPECT_EQ(a->formula(), nullptr);
}

TEST_F(AttributeTest, InsertDerivationFormulaSucceeds)
{
  auto a = std::make_shared<Attribute>();
  EXPECT_NO_THROW(
      a->insertChild(0, std::make_shared<ScaledSumDerivationFormula>(QList<int>{1}, 1)));
}

TEST_F(AttributeTest, InsertDerivationFormulaPlacesItAtIndexZero)
{
  auto a = std::make_shared<Attribute>();
  a->insertChild(0, std::make_shared<ScaledSumDerivationFormula>(QList<int>{1}, 1));
  EXPECT_NE(dynamic_cast<Formula*>(a->childAt(0).get()), nullptr);
}

TEST_F(AttributeTest, InsertDerivationFormulaTwiceDoesNotThrow)
{
  // With direct TreeItem API, duplicate checks are responsibility of caller
  auto a = std::make_shared<Attribute>();
  a->insertChild(0, std::make_shared<ScaledSumDerivationFormula>(QList<int>{1}, 1));
  EXPECT_NO_THROW(
      a->insertChild(0, std::make_shared<ScaledSumDerivationFormula>(QList<int>{1}, 1)));
}

TEST_F(AttributeTest, FormulaObjectReturnsDerivationFormula)
{
  auto a = std::make_shared<Attribute>();
  a->insertChild(0, std::make_shared<ScaledSumDerivationFormula>(QList<int>{1}, 1));
  EXPECT_NE(dynamic_cast<ScaledSumDerivationFormula*>(a->formula()), nullptr);
}

TEST_F(AttributeTest, DerivationFormulaRemainsAtIndexZeroAfterDirectFormulaInserted)
{
  auto a = std::make_shared<Attribute>();
  a->insertChild(0, std::make_shared<ScaledSumDerivationFormula>(QList<int>{1}, 1));
  a->insertChild(a->size(), std::make_shared<LinearFormula>(5));
  EXPECT_NE(dynamic_cast<Formula*>(a->childAt(0).get()), nullptr);
}

// ── Parent references ─────────────────────────────────────────────────────────

TEST_F(AttributeTest, ParentCountIsZeroOnFreshAttribute)
{
  // Count ParentRef children via TreeItem interface
  auto a = std::make_shared<Attribute>();
  int count = 0;
  for (int i = 0; i < a->size(); ++i) {
    if (dynamic_cast<ParentRef*>(a->childAt(i).get())) {
      ++count;
    }
  }
  EXPECT_EQ(count, 0);
}

TEST_F(AttributeTest, InsertParentIncreasesParentCount)
{
  auto a = std::make_shared<Attribute>();
  // Insert ParentRef after derivation formula (if present), before direct formula (if present)
  a->insertChild(a->size(), std::make_shared<ParentRef>());
  int count = 0;
  for (int i = 0; i < a->size(); ++i) {
    if (dynamic_cast<ParentRef*>(a->childAt(i).get())) {
      ++count;
    }
  }
  EXPECT_EQ(count, 1);
}

TEST_F(AttributeTest, InsertTwoParentsGivesParentCountTwo)
{
  auto a = std::make_shared<Attribute>();
  a->insertChild(a->size(), std::make_shared<ParentRef>());
  a->insertChild(a->size(), std::make_shared<ParentRef>());
  int count = 0;
  for (int i = 0; i < a->size(); ++i) {
    if (dynamic_cast<ParentRef*>(a->childAt(i).get())) {
      ++count;
    }
  }
  EXPECT_EQ(count, 2);
}

TEST_F(AttributeTest, ParentRefAccessibleViaTreeItemInterface)
{
  auto a = std::make_shared<Attribute>();
  auto ref = std::make_shared<ParentRef>();
  ref->setTargetIdString("aaaaaaaa-bbbb-4ccc-8ddd-eeeeeeeeeeee");
  a->insertChild(a->size(), ref);

  // Find the ParentRef child
  ParentRef* foundRef = nullptr;
  for (int i = 0; i < a->size(); ++i) {
    if (auto* pr = dynamic_cast<ParentRef*>(a->childAt(i).get())) {
      foundRef = pr;
      break;
    }
  }
  ASSERT_NE(foundRef, nullptr);
  EXPECT_EQ(foundRef->targetIdString(), "aaaaaaaa-bbbb-4ccc-8ddd-eeeeeeeeeeee");
}

TEST_F(AttributeTest, ParentInsertedBeforeDirectFormula)
{
  // Layout: [DerivationFormula, ParentRef, DirectFormula]
  auto a = std::make_shared<Attribute>();
  a->insertChild(0, std::make_shared<ScaledSumDerivationFormula>(QList<int>{1}, 1));
  a->insertChild(a->size(), std::make_shared<LinearFormula>(5));
  // Insert ParentRef between derivation and direct formula
  a->insertChild(1, std::make_shared<ParentRef>());
  ASSERT_EQ(a->size(), 3);
  EXPECT_NE(dynamic_cast<Formula*>(a->childAt(0).get()), nullptr);
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
