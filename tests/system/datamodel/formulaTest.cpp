#include <formula.hpp>
#include <linearFormula.hpp>
#include <lookupFormula.hpp>

#include <gtest/gtest.h>

#include <stdexcept>

using namespace gurps_system;

// ══════════════════════════════════════════════════════════════════════════════
// Formula (abstract base identity)
// ══════════════════════════════════════════════════════════════════════════════

TEST(FormulaTest, ClassIdIsNonNil) { EXPECT_FALSE(Formula::classId().is_nil()); }

TEST(FormulaTest, ClassIdIsStable) { EXPECT_EQ(Formula::classId(), Formula::classId()); }

TEST(FormulaTest, ClassIdDiffersFromTreeItemClassId)
{
  EXPECT_NE(Formula::classId(), infrastructure::TreeItem::classId());
}

// ══════════════════════════════════════════════════════════════════════════════
// LinearFormula
// ══════════════════════════════════════════════════════════════════════════════

// ── Identity ───────────────────────────────────────────────────────────────────────
TEST(LinearFormulaTest, ClassIdIsNonNil) { EXPECT_FALSE(LinearFormula::classId().is_nil()); }

TEST(LinearFormulaTest, ClassIdDiffersFromFormulaClassId)
{
  EXPECT_NE(LinearFormula::classId(), Formula::classId());
}

TEST(LinearFormulaTest, TypeIdMatchesClassId)
{
  LinearFormula f{10};
  EXPECT_EQ(f.typeId(), LinearFormula::classId());
}

TEST(LinearFormulaTest, IsATreeItem)
{
  auto f = std::make_shared<LinearFormula>(10);
  EXPECT_NE(dynamic_cast<infrastructure::TreeItem*>(f.get()), nullptr);
}

// ── Construction ──────────────────────────────────────────────────────────────

TEST(LinearFormulaTest, ConstructionStoresCostPerLevel)
{
  LinearFormula f{10};
  EXPECT_EQ(f.costPerLevel(), 10);
}

TEST(LinearFormulaTest, ZeroCostPerLevelThrows)
{
  EXPECT_THROW(LinearFormula{0}, std::invalid_argument);
}

TEST(LinearFormulaTest, NegativeCostPerLevelThrows)
{
  EXPECT_THROW(LinearFormula{-5}, std::invalid_argument);
}

// ── levelBonus ────────────────────────────────────────────────────────────────

TEST(LinearFormulaTest, ZeroCpGivesZeroBonus)
{
  LinearFormula f{10};
  EXPECT_EQ(f.levelBonus(0), 0);
}

TEST(LinearFormulaTest, ExactOneLevelCpGivesOneBonus)
{
  LinearFormula f{10};
  EXPECT_EQ(f.levelBonus(10), 1);
}

TEST(LinearFormulaTest, ExactTwoLevelCpGivesTwoBonus)
{
  LinearFormula f{10};
  EXPECT_EQ(f.levelBonus(20), 2);
}

TEST(LinearFormulaTest, PartialLevelCpGivesFlooredBonus)
{
  LinearFormula f{10};
  EXPECT_EQ(f.levelBonus(15), 1); // 15/10 truncates to 1
  EXPECT_EQ(f.levelBonus(9), 0);  // 9/10 truncates to 0
}

TEST(LinearFormulaTest, LargeCpScalesCorrectly)
{
  LinearFormula f{20}; // DX/IQ cost
  EXPECT_EQ(f.levelBonus(80), 4);
}

// ── cpCost ────────────────────────────────────────────────────────────────────

TEST(LinearFormulaTest, ZeroBonusCostsZero)
{
  LinearFormula f{10};
  EXPECT_EQ(f.cpCost(0), 0);
}

TEST(LinearFormulaTest, OneBonusCostsCostPerLevel)
{
  LinearFormula f{10};
  EXPECT_EQ(f.cpCost(1), 10);
}

TEST(LinearFormulaTest, ThreeBonusCostsThreeTimesCostPerLevel)
{
  LinearFormula f{10};
  EXPECT_EQ(f.cpCost(3), 30);
}

// ── Round-trip ────────────────────────────────────────────────────────────────

TEST(LinearFormulaTest, RoundTripLevelBonusThenCpCost)
{
  LinearFormula f{10};
  for (int bonus = 0; bonus <= 10; ++bonus) {
    EXPECT_EQ(f.levelBonus(f.cpCost(bonus)), bonus) << "bonus=" << bonus;
  }
}

// ── setCostPerLevel ────────────────────────────────────────────────────────────────────

TEST(LinearFormulaTest, SetCostPerLevelUpdatesValue)
{
  LinearFormula f{10};
  f.setCostPerLevel(20);
  EXPECT_EQ(f.costPerLevel(), 20);
}

TEST(LinearFormulaTest, SetCostPerLevelZeroThrows)
{
  LinearFormula f{10};
  EXPECT_THROW(f.setCostPerLevel(0), std::invalid_argument);
}

TEST(LinearFormulaTest, SetCostPerLevelNegativeThrows)
{
  LinearFormula f{10};
  EXPECT_THROW(f.setCostPerLevel(-1), std::invalid_argument);
}

TEST(LinearFormulaTest, SetCostPerLevelInvalidDoesNotChangeValue)
{
  LinearFormula f{10};
  EXPECT_THROW(f.setCostPerLevel(0), std::invalid_argument);
  EXPECT_EQ(f.costPerLevel(), 10); // unchanged
}

// ══════════════════════════════════════════════════════════════════════════════
// LookupFormula
// ══════════════════════════════════════════════════════════════════════════════

namespace {
// A small test table modelling a non-linear schedule:
//  bonus 0 →  0 CP,  bonus 1 → 10 CP,  bonus 2 → 25 CP,  bonus 3 → 45 CP
std::map<int, int> const sampleTable{{0, 0}, {1, 10}, {2, 25}, {3, 45}};
} // namespace
// ── Identity ───────────────────────────────────────────────────────────────────────
TEST(LookupFormulaTest, ClassIdIsNonNil) { EXPECT_FALSE(LookupFormula::classId().is_nil()); }

TEST(LookupFormulaTest, ClassIdDiffersFromLinearFormulaClassId)
{
  EXPECT_NE(LookupFormula::classId(), LinearFormula::classId());
}

TEST(LookupFormulaTest, TypeIdMatchesClassId)
{
  LookupFormula f{sampleTable};
  EXPECT_EQ(f.typeId(), LookupFormula::classId());
}

TEST(LookupFormulaTest, IsATreeItem)
{
  auto f = std::make_shared<LookupFormula>(sampleTable);
  EXPECT_NE(dynamic_cast<infrastructure::TreeItem*>(f.get()), nullptr);
}
// ── Construction ──────────────────────────────────────────────────────────────

TEST(LookupFormulaTest, ConstructionStoresTable)
{
  LookupFormula f{sampleTable};
  EXPECT_EQ(f.table(), sampleTable);
}

TEST(LookupFormulaTest, MissingZeroEntryThrows)
{
  EXPECT_THROW(LookupFormula({{1, 10}, {2, 25}}), std::invalid_argument);
}

TEST(LookupFormulaTest, WrongZeroEntryValueThrows)
{
  // Entry {0, 5} — zero bonus must cost zero
  EXPECT_THROW(LookupFormula({{0, 5}, {1, 10}}), std::invalid_argument);
}

// ── levelBonus ────────────────────────────────────────────────────────────────

TEST(LookupFormulaTest, ZeroCpGivesZeroBonus)
{
  LookupFormula f{sampleTable};
  EXPECT_EQ(f.levelBonus(0), 0);
}

TEST(LookupFormulaTest, ExactEntryReturnsCorrectBonus)
{
  LookupFormula f{sampleTable};
  EXPECT_EQ(f.levelBonus(10), 1);
  EXPECT_EQ(f.levelBonus(25), 2);
  EXPECT_EQ(f.levelBonus(45), 3);
}

TEST(LookupFormulaTest, BetweenEntriesReturnsLowerBonus)
{
  LookupFormula f{sampleTable};
  EXPECT_EQ(f.levelBonus(11), 1); // between 10 and 25 → still bonus 1
  EXPECT_EQ(f.levelBonus(24), 1);
  EXPECT_EQ(f.levelBonus(30), 2); // between 25 and 45 → still bonus 2
}

TEST(LookupFormulaTest, BelowFirstPositiveEntryReturnsZero)
{
  LookupFormula f{sampleTable};
  EXPECT_EQ(f.levelBonus(5), 0);
  EXPECT_EQ(f.levelBonus(1), 0);
}

TEST(LookupFormulaTest, AboveMaxEntryReturnsMaxBonus)
{
  LookupFormula f{sampleTable};
  EXPECT_EQ(f.levelBonus(999), 3);
}

// ── cpCost ────────────────────────────────────────────────────────────────────

TEST(LookupFormulaTest, ZeroBonusCostsZero)
{
  LookupFormula f{sampleTable};
  EXPECT_EQ(f.cpCost(0), 0);
}

TEST(LookupFormulaTest, MappedBonusReturnsCorrectCost)
{
  LookupFormula f{sampleTable};
  EXPECT_EQ(f.cpCost(1), 10);
  EXPECT_EQ(f.cpCost(2), 25);
  EXPECT_EQ(f.cpCost(3), 45);
}

TEST(LookupFormulaTest, UnmappedBonusThrowsOutOfRange)
{
  LookupFormula f{sampleTable};
  EXPECT_THROW(f.cpCost(99), std::out_of_range);
}

// ── Round-trip ────────────────────────────────────────────────────────────────

TEST(LookupFormulaTest, RoundTripLevelBonusThenCpCost)
{
  LookupFormula f{sampleTable};
  for (int bonus = 0; bonus <= 3; ++bonus) {
    EXPECT_EQ(f.levelBonus(f.cpCost(bonus)), bonus) << "bonus=" << bonus;
  }
}

// ── setTable ───────────────────────────────────────────────────────────────────────

TEST(LookupFormulaTest, SetTableUpdatesTable)
{
  LookupFormula f{sampleTable};
  std::map<int, int> const newTable{{0, 0}, {1, 5}, {2, 15}};
  f.setTable(newTable);
  EXPECT_EQ(f.table(), newTable);
}

TEST(LookupFormulaTest, SetTableUpdatesLookupBehaviour)
{
  LookupFormula f{sampleTable};
  f.setTable({{0, 0}, {1, 5}, {2, 15}});
  EXPECT_EQ(f.levelBonus(5), 1);
  EXPECT_EQ(f.levelBonus(15), 2);
}

TEST(LookupFormulaTest, SetTableMissingZeroEntryThrows)
{
  LookupFormula f{sampleTable};
  EXPECT_THROW(f.setTable({{1, 10}, {2, 25}}), std::invalid_argument);
}

TEST(LookupFormulaTest, SetTableWrongZeroEntryValueThrows)
{
  LookupFormula f{sampleTable};
  EXPECT_THROW(f.setTable({{0, 5}, {1, 10}}), std::invalid_argument);
}

TEST(LookupFormulaTest, SetTableInvalidDoesNotChangeTable)
{
  LookupFormula f{sampleTable};
  EXPECT_THROW(f.setTable({{1, 10}}), std::invalid_argument);
  EXPECT_EQ(f.table(), sampleTable); // unchanged
}
