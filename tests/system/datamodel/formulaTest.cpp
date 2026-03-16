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

// ── setTable ───────────────────────────────────────────────────────────────────────

TEST(LookupFormulaTest, SetTableUpdatesTable)
{
  LookupFormula f{sampleTable};
  std::map<int, int> const newTable{{0, 0}, {1, 5}, {2, 15}};
  f.setTable(newTable);
  EXPECT_EQ(f.table(), newTable);
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
