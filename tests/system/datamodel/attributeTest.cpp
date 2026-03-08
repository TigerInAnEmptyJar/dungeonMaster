#include <attribute.hpp>
#include <baseObject.hpp>
#include <formula.hpp>
#include <linearFormula.hpp>
#include <lookupFormula.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

// ── Qt event loop ─────────────────────────────────────────────────────────────
// QCoreApplication is provided by baseObjectTest.cpp (same test executable).

using namespace gurps_system;

// ── Helpers ───────────────────────────────────────────────────────────────────

// Creates a heap-managed Attribute with a 10-CP/level linear formula attached.
static auto makeAttrWithFormula() -> std::shared_ptr<Attribute>
{
  auto attr = std::make_shared<Attribute>();
  attr->insertChild(0, std::make_shared<LinearFormula>(10));
  return attr;
}

// ── Fixture ───────────────────────────────────────────────────────────────────

class AttributeTest : public ::testing::Test
{
protected:
  // Attribute with a 10 CP-per-level linear formula (GURPS 4e ST/HT pattern)
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

// ── Creation without formula ──────────────────────────────────────────────────

TEST_F(AttributeTest, CreateWithoutFormulaSucceeds)
{
  EXPECT_NO_THROW(std::make_shared<Attribute>());
}

TEST_F(AttributeTest, LevelThrowsWhenNoFormulaChild)
{
  auto attr = std::make_shared<Attribute>();
  EXPECT_THROW(attr->level(0), std::logic_error);
}

TEST_F(AttributeTest, CpForLevelBonusThrowsWhenNoFormulaChild)
{
  auto attr = std::make_shared<Attribute>();
  EXPECT_THROW(attr->cpForLevelBonus(1), std::logic_error);
}

// ── level() ───────────────────────────────────────────────────────────────────

TEST_F(AttributeTest, LevelWithZeroCpIsTen)
{
  EXPECT_EQ(attr->level(0), 10); // base=10, bonus=0
}

TEST_F(AttributeTest, LevelIncreasesWithInvestedCp)
{
  EXPECT_EQ(attr->level(10), 11); // formula: 10/10 = bonus 1 → level 11
}

TEST_F(AttributeTest, LevelEqualsBasePlusFormulaBonus)
{
  EXPECT_EQ(attr->level(20), 12); // base 10 + bonus 2
}

TEST_F(AttributeTest, LevelWithNegativeCpDecreasesLevel)
{
  EXPECT_EQ(attr->level(-10), 9); // formula: -10/10 = bonus -1 → level 9
}

// ── cpForLevelBonus ───────────────────────────────────────────────────────────

TEST_F(AttributeTest, CpForLevelBonusDelegatesToFormula)
{
  EXPECT_EQ(attr->cpForLevelBonus(3), 30); // 3 * costPerLevel(10)
}

// ── Formula as first child ────────────────────────────────────────────────────

TEST_F(AttributeTest, FormulaIsFirstChild) { EXPECT_NE(attr->childAt(0), nullptr); }

TEST_F(AttributeTest, FirstChildIsAFormula)
{
  EXPECT_NE(dynamic_cast<Formula*>(attr->childAt(0).get()), nullptr);
}

// ── Formula independence ──────────────────────────────────────────────────────

TEST_F(AttributeTest, TwoAttributesWithDifferentFormulasDifferInLevel)
{
  // DX/IQ formula: 20 CP per level
  auto dxAttr = std::make_shared<Attribute>();
  dxAttr->insertChild(0, std::make_shared<LinearFormula>(20));

  EXPECT_EQ(attr->level(20), 12);   // ST formula: bonus = 20/10 = 2 → level 12
  EXPECT_EQ(dxAttr->level(20), 11); // DX formula: bonus = 20/20 = 1 → level 11
}

TEST_F(AttributeTest, LookupFormulaWorksWithAttribute)
{
  auto a = std::make_shared<Attribute>();
  a->insertChild(
      0, std::make_shared<LookupFormula>(std::map<int, int>{{0, 0}, {1, 10}, {2, 25}, {3, 45}}));

  EXPECT_EQ(a->level(25), 12); // base 10 + bonus 2
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
