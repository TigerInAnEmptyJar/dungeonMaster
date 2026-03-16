#include <registration.hpp>

#include <attribute.hpp>
#include <objectFactory.hpp>
#include <parentRef.hpp>
#include <profession.hpp>
#include <race.hpp>
#include <skill.hpp>

#include <linearFormula.hpp>
#include <lookupDerivationFormula.hpp>
#include <lookupFormula.hpp>
#include <quadraticDerivationFormula.hpp>
#include <scaledSumDerivationFormula.hpp>

#include <boost/uuid/string_generator.hpp>

#include <gtest/gtest.h>

// ── Qt event loop ─────────────────────────────────────────────────────────────
// QCoreApplication is provided by baseObjectTest.cpp (same test executable).

using namespace gurps_system;

// ── Fixture ───────────────────────────────────────────────────────────────────

class RegistrationTest : public ::testing::Test
{
protected:
  infrastructure::ObjectFactory factory;

  RegistrationTest() { registerSystemObjects(factory); }
};

// ── Registration coverage ─────────────────────────────────────────────────────

TEST_F(RegistrationTest, AttributeIsRegistered)
{
  EXPECT_TRUE(factory.isInstalled(Attribute::classId()));
}

TEST_F(RegistrationTest, LinearFormulaIsRegistered)
{
  EXPECT_TRUE(factory.isInstalled(LinearFormula::classId()));
}

TEST_F(RegistrationTest, LookupFormulaIsRegistered)
{
  EXPECT_TRUE(factory.isInstalled(LookupFormula::classId()));
}

TEST_F(RegistrationTest, ParentRefIsRegistered)
{
  EXPECT_TRUE(factory.isInstalled(ParentRef::classId()));
}

TEST_F(RegistrationTest, ScaledSumDerivationFormulaIsRegistered)
{
  EXPECT_TRUE(factory.isInstalled(ScaledSumDerivationFormula::classId()));
}

TEST_F(RegistrationTest, QuadraticDerivationFormulaIsRegistered)
{
  EXPECT_TRUE(factory.isInstalled(QuadraticDerivationFormula::classId()));
}

TEST_F(RegistrationTest, LookupDerivationFormulaIsRegistered)
{
  EXPECT_TRUE(factory.isInstalled(LookupDerivationFormula::classId()));
}

TEST_F(RegistrationTest, RaceIsRegistered) { EXPECT_TRUE(factory.isInstalled(Race::classId())); }

TEST_F(RegistrationTest, ProfessionIsRegistered)
{
  EXPECT_TRUE(factory.isInstalled(Profession::classId()));
}

TEST_F(RegistrationTest, SkillIsRegistered) { EXPECT_TRUE(factory.isInstalled(Skill::classId())); }

TEST_F(RegistrationTest, AllTypesRegistered) { EXPECT_EQ(factory.installedCount(), 18); }

// ── Factory creation ──────────────────────────────────────────────────────────

TEST_F(RegistrationTest, CreateAttributeReturnsNonNull)
{
  EXPECT_NE(factory.create(Attribute::classId()), nullptr);
}

TEST_F(RegistrationTest, CreateAttributeReturnsAttribute)
{
  auto obj = factory.create(Attribute::classId());
  EXPECT_NE(dynamic_cast<Attribute*>(obj.get()), nullptr);
}

TEST_F(RegistrationTest, CreateAttributeUsesProvidedObjectId)
{
  auto id = boost::uuids::string_generator{}("aaaaaaaa-bbbb-4ccc-8ddd-eeeeeeeeeeee");
  auto obj = factory.create(Attribute::classId(), id);
  EXPECT_EQ(obj->objectId(), id);
}

TEST_F(RegistrationTest, CreateLinearFormulaReturnsLinearFormula)
{
  auto obj = factory.create(LinearFormula::classId());
  EXPECT_NE(dynamic_cast<LinearFormula*>(obj.get()), nullptr);
}

TEST_F(RegistrationTest, CreateLookupFormulaReturnsLookupFormula)
{
  auto obj = factory.create(LookupFormula::classId());
  EXPECT_NE(dynamic_cast<LookupFormula*>(obj.get()), nullptr);
}

TEST_F(RegistrationTest, CreateParentRefReturnsParentRef)
{
  auto obj = factory.create(ParentRef::classId());
  EXPECT_NE(dynamic_cast<ParentRef*>(obj.get()), nullptr);
}

TEST_F(RegistrationTest, CreateScaledSumDerivationFormulaReturnsCorrectType)
{
  auto obj = factory.create(ScaledSumDerivationFormula::classId());
  EXPECT_NE(dynamic_cast<ScaledSumDerivationFormula*>(obj.get()), nullptr);
}

TEST_F(RegistrationTest, CreateQuadraticDerivationFormulaReturnsCorrectType)
{
  auto obj = factory.create(QuadraticDerivationFormula::classId());
  EXPECT_NE(dynamic_cast<QuadraticDerivationFormula*>(obj.get()), nullptr);
}

TEST_F(RegistrationTest, CreateLookupDerivationFormulaReturnsCorrectType)
{
  auto obj = factory.create(LookupDerivationFormula::classId());
  EXPECT_NE(dynamic_cast<LookupDerivationFormula*>(obj.get()), nullptr);
}

// ── Idempotency / double-registration ────────────────────────────────────────

TEST_F(RegistrationTest, CallingRegisterTwiceDoesNotAddDuplicates)
{
  registerSystemObjects(factory); // second call — install() returns false, no throw
  EXPECT_EQ(factory.installedCount(), 18);
}
