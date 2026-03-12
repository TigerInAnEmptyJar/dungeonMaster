#include <derivationFormula.hpp>
#include <lookupDerivationFormula.hpp>
#include <quadraticDerivationFormula.hpp>
#include <scaledSumDerivationFormula.hpp>

#include <cpTable.hpp>

#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>

// ── Qt event loop ─────────────────────────────────────────────────────────────
// QCoreApplication is provided by baseObjectTest.cpp (same test executable).

using namespace gurps_system;

// ── ScaledSumDerivationFormula ────────────────────────────────────────────────

TEST(ScaledSumDerivationFormulaTest, ClassIdIsNonNil)
{
  EXPECT_FALSE(ScaledSumDerivationFormula::classId().is_nil());
}

TEST(ScaledSumDerivationFormulaTest, ClassIdIsStable)
{
  EXPECT_EQ(ScaledSumDerivationFormula::classId(), ScaledSumDerivationFormula::classId());
}

TEST(ScaledSumDerivationFormulaTest, ClassIdDiffersFromDerivationFormulaClassId)
{
  EXPECT_NE(ScaledSumDerivationFormula::classId(), DerivationFormula::classId());
}

TEST(ScaledSumDerivationFormulaTest, TypeIdMatchesClassId)
{
  auto f = std::make_shared<ScaledSumDerivationFormula>(QList<int>{1, 1}, 4);
  EXPECT_EQ(f->typeId(), ScaledSumDerivationFormula::classId());
}

TEST(ScaledSumDerivationFormulaTest, ConstructionWithValidArgsSucceeds)
{
  EXPECT_NO_THROW(std::make_shared<ScaledSumDerivationFormula>(QList<int>{1, 1}, 4));
}

TEST(ScaledSumDerivationFormulaTest, ConstructionWithZeroDivisorThrows)
{
  EXPECT_THROW(std::make_shared<ScaledSumDerivationFormula>(QList<int>{1}, 0),
               std::invalid_argument);
}

TEST(ScaledSumDerivationFormulaTest, ConstructionWithNegativeDivisorThrows)
{
  EXPECT_THROW(std::make_shared<ScaledSumDerivationFormula>(QList<int>{1}, -1),
               std::invalid_argument);
}

TEST(ScaledSumDerivationFormulaTest, ConstructionWithEmptyCoefficientsThrows)
{
  EXPECT_THROW(std::make_shared<ScaledSumDerivationFormula>(QList<int>{}, 1),
               std::invalid_argument);
}

TEST(ScaledSumDerivationFormulaTest, CoefficientsAreReadBack)
{
  QList<int> const coeff{2, 3};
  auto f = std::make_shared<ScaledSumDerivationFormula>(coeff, 4);
  EXPECT_EQ(f->coefficients(), coeff);
}

TEST(ScaledSumDerivationFormulaTest, DivisorIsReadBack)
{
  auto f = std::make_shared<ScaledSumDerivationFormula>(QList<int>{1, 1}, 4);
  EXPECT_EQ(f->divisor(), 4);
}

TEST(ScaledSumDerivationFormulaTest, SetCoefficientsUpdatesValue)
{
  auto f = std::make_shared<ScaledSumDerivationFormula>(QList<int>{1}, 1);
  f->setCoefficients(QList<int>{3, 3});
  EXPECT_EQ(f->coefficients(), (QList<int>{3, 3}));
}

TEST(ScaledSumDerivationFormulaTest, SetCoefficientsEmptyThrows)
{
  auto f = std::make_shared<ScaledSumDerivationFormula>(QList<int>{1}, 1);
  EXPECT_THROW(f->setCoefficients(QList<int>{}), std::invalid_argument);
}

TEST(ScaledSumDerivationFormulaTest, SetDivisorUpdatesValue)
{
  auto f = std::make_shared<ScaledSumDerivationFormula>(QList<int>{1}, 1);
  f->setDivisor(5);
  EXPECT_EQ(f->divisor(), 5);
}

TEST(ScaledSumDerivationFormulaTest, SetDivisorZeroThrows)
{
  auto f = std::make_shared<ScaledSumDerivationFormula>(QList<int>{1}, 1);
  EXPECT_THROW(f->setDivisor(0), std::invalid_argument);
}

// ── QuadraticDerivationFormula ────────────────────────────────────────────────

TEST(QuadraticDerivationFormulaTest, ClassIdIsNonNil)
{
  EXPECT_FALSE(QuadraticDerivationFormula::classId().is_nil());
}

TEST(QuadraticDerivationFormulaTest, ClassIdDiffersFromScaledSum)
{
  EXPECT_NE(QuadraticDerivationFormula::classId(), ScaledSumDerivationFormula::classId());
}

TEST(QuadraticDerivationFormulaTest, TypeIdMatchesClassId)
{
  auto f = std::make_shared<QuadraticDerivationFormula>(5);
  EXPECT_EQ(f->typeId(), QuadraticDerivationFormula::classId());
}

TEST(QuadraticDerivationFormulaTest, ConstructionWithValidDivisorSucceeds)
{
  EXPECT_NO_THROW(std::make_shared<QuadraticDerivationFormula>(5));
}

TEST(QuadraticDerivationFormulaTest, ConstructionWithZeroDivisorThrows)
{
  EXPECT_THROW(std::make_shared<QuadraticDerivationFormula>(0), std::invalid_argument);
}

TEST(QuadraticDerivationFormulaTest, DivisorIsReadBack)
{
  auto f = std::make_shared<QuadraticDerivationFormula>(5);
  EXPECT_EQ(f->divisor(), 5);
}

TEST(QuadraticDerivationFormulaTest, SetDivisorUpdatesValue)
{
  auto f = std::make_shared<QuadraticDerivationFormula>(1);
  f->setDivisor(10);
  EXPECT_EQ(f->divisor(), 10);
}

TEST(QuadraticDerivationFormulaTest, SetDivisorZeroThrows)
{
  auto f = std::make_shared<QuadraticDerivationFormula>(1);
  EXPECT_THROW(f->setDivisor(0), std::invalid_argument);
}

// ── LookupDerivationFormula ───────────────────────────────────────────────────

TEST(LookupDerivationFormulaTest, ClassIdIsNonNil)
{
  EXPECT_FALSE(LookupDerivationFormula::classId().is_nil());
}

TEST(LookupDerivationFormulaTest, ClassIdDiffersFromOtherFormulas)
{
  EXPECT_NE(LookupDerivationFormula::classId(), ScaledSumDerivationFormula::classId());
  EXPECT_NE(LookupDerivationFormula::classId(), QuadraticDerivationFormula::classId());
}

TEST(LookupDerivationFormulaTest, TypeIdMatchesClassId)
{
  auto f = std::make_shared<LookupDerivationFormula>(CpTable{{1, 10}});
  EXPECT_EQ(f->typeId(), LookupDerivationFormula::classId());
}

TEST(LookupDerivationFormulaTest, ConstructionWithNonEmptyTableSucceeds)
{
  EXPECT_NO_THROW(std::make_shared<LookupDerivationFormula>(CpTable{{0, 5}, {1, 10}}));
}

TEST(LookupDerivationFormulaTest, ConstructionWithEmptyTableThrows)
{
  EXPECT_THROW(std::make_shared<LookupDerivationFormula>(CpTable{}), std::invalid_argument);
}

TEST(LookupDerivationFormulaTest, TableIsReadBack)
{
  CpTable const t{{1, 10}, {2, 15}};
  auto f = std::make_shared<LookupDerivationFormula>(t);
  EXPECT_EQ(f->table(), t);
}

TEST(LookupDerivationFormulaTest, SetTableUpdatesValue)
{
  auto f = std::make_shared<LookupDerivationFormula>(CpTable{{0, 0}});
  CpTable const newTable{{1, 7}, {2, 14}};
  f->setTable(newTable);
  EXPECT_EQ(f->table(), newTable);
}

TEST(LookupDerivationFormulaTest, SetTableEmptyThrows)
{
  auto f = std::make_shared<LookupDerivationFormula>(CpTable{{0, 0}});
  EXPECT_THROW(f->setTable(CpTable{}), std::invalid_argument);
}

// ── No sentinel requirement unlike LookupFormula ─────────────────────────────

TEST(LookupDerivationFormulaTest, TableWithoutZeroKeyIsValid)
{
  EXPECT_NO_THROW(std::make_shared<LookupDerivationFormula>(CpTable{{5, 1}, {10, 2}}));
}
