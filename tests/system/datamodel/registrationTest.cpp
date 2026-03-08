#include <registration.hpp>

#include <attribute.hpp>
#include <linearFormula.hpp>
#include <lookupFormula.hpp>
#include <objectFactory.hpp>

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

TEST_F(RegistrationTest, AllThreeTypesRegistered) { EXPECT_EQ(factory.installedCount(), 3); }

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

// ── Idempotency / double-registration ────────────────────────────────────────

TEST_F(RegistrationTest, CallingRegisterTwiceDoesNotAddDuplicates)
{
  registerSystemObjects(factory); // second call — install() returns false, no throw
  EXPECT_EQ(factory.installedCount(), 3);
}
