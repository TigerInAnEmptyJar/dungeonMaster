#include <attribute.hpp>
#include <cpTable.hpp>
#include <jsonSerializer.hpp>
#include <linearFormula.hpp>
#include <lookupDerivationFormula.hpp>
#include <objectFactory.hpp>
#include <objectRegistry.hpp>
#include <parentRef.hpp>
#include <quadraticDerivationFormula.hpp>
#include <registration.hpp>
#include <scaledSumDerivationFormula.hpp>
#include <serializer.hpp>
#include <xmlSerializer.hpp>

#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <gtest/gtest.h>

#include <memory>
#include <sstream>
#include <string>
#include <vector>

// ── QCoreApplication is provided by ioTest.cpp (same test binary) ─────────────

using namespace gurps_system;

// ── Stable object UUIDs ───────────────────────────────────────────────────────

namespace ids {

static auto st()
{
  static auto v = boost::uuids::string_generator{}("a0000001-0000-4000-8000-000000000000");
  return v;
}
static auto dx()
{
  static auto v = boost::uuids::string_generator{}("a0000002-0000-4000-8000-000000000000");
  return v;
}
static auto iq()
{
  static auto v = boost::uuids::string_generator{}("a0000003-0000-4000-8000-000000000000");
  return v;
}
static auto ht()
{
  static auto v = boost::uuids::string_generator{}("a0000004-0000-4000-8000-000000000000");
  return v;
}
static auto hp()
{
  static auto v = boost::uuids::string_generator{}("b0000001-0000-4000-8000-000000000000");
  return v;
}
static auto will()
{
  static auto v = boost::uuids::string_generator{}("b0000002-0000-4000-8000-000000000000");
  return v;
}
static auto per()
{
  static auto v = boost::uuids::string_generator{}("b0000003-0000-4000-8000-000000000000");
  return v;
}
static auto fp()
{
  static auto v = boost::uuids::string_generator{}("b0000004-0000-4000-8000-000000000000");
  return v;
}
static auto basicSpeed()
{
  static auto v = boost::uuids::string_generator{}("b0000005-0000-4000-8000-000000000000");
  return v;
}
static auto basicMove()
{
  static auto v = boost::uuids::string_generator{}("b0000006-0000-4000-8000-000000000000");
  return v;
}
static auto basicLift()
{
  static auto v = boost::uuids::string_generator{}("b0000007-0000-4000-8000-000000000000");
  return v;
}
static auto thrustDmg()
{
  static auto v = boost::uuids::string_generator{}("b0000008-0000-4000-8000-000000000000");
  return v;
}
static auto swingDmg()
{
  static auto v = boost::uuids::string_generator{}("b0000009-0000-4000-8000-000000000000");
  return v;
}

} // namespace ids

// ── GURPS 4e damage tables (ST 1–20) ─────────────────────────────────────────
//
// Thrust and Swing values are encoded as an integer offset relative to 1d6
// (negative = below 1d6 potential, positive = above). This is a system-layer
// representation; the character layer interprets the exact dice expression.

// clang-format off
static CpTable const k_thrustTable{
  {1,-5},{2,-5},{3,-4},{4,-4},{5,-3},{6,-3},{7,-2},{8,-2},{9,-1},{10,0},
  {11,0},{12,1},{13,1},{14,2},{15,2},{16,3},{17,3},{18,4},{19,4},{20,5}
};
static CpTable const k_swingTable{
  {1,-3},{2,-3},{3,-2},{4,-2},{5,-1},{6,0},{7,0},{8,1},{9,1},{10,2},
  {11,2},{12,3},{13,3},{14,4},{15,4},{16,5},{17,6},{18,6},{19,7},{20,8}
};
// clang-format on

// ── Fixture data ──────────────────────────────────────────────────────────────

struct Gurps4Setup
{
  // Basic attributes
  std::shared_ptr<Attribute> st;
  std::shared_ptr<Attribute> dx;
  std::shared_ptr<Attribute> iq;
  std::shared_ptr<Attribute> ht;

  // Secondary attributes
  std::shared_ptr<Attribute> hp;
  std::shared_ptr<Attribute> will;
  std::shared_ptr<Attribute> per;
  std::shared_ptr<Attribute> fp;
  std::shared_ptr<Attribute> basicSpeed;
  std::shared_ptr<Attribute> basicMove;
  std::shared_ptr<Attribute> basicLift;
  std::shared_ptr<Attribute> thrustDmg;
  std::shared_ptr<Attribute> swingDmg;

  auto items() const -> std::vector<std::shared_ptr<infrastructure::TreeItem>>
  {
    return {st, dx,         iq,        ht,        hp,        will,    per,
            fp, basicSpeed, basicMove, basicLift, thrustDmg, swingDmg};
  }
};

// Builds a LinearFormula-backed Attribute.
static auto makeBasicAttr(std::string const& name, int cpPerLevel, boost::uuids::uuid id)
    -> std::shared_ptr<Attribute>
{
  auto attr = std::make_shared<Attribute>(id);
  attr->setName(QString::fromStdString(name));
  attr->insertChild(attr->size(), std::make_shared<LinearFormula>(cpPerLevel));
  return attr;
}

// Builds an Attribute driven by a ScaledSumDerivationFormula
// with the given parent targets and coefficient (all 1) and divisor.
static auto makeScaledSecondary(std::string const& name, QList<int> const& coefficients,
                                int divisor, std::vector<boost::uuids::uuid> const& parentIds,
                                boost::uuids::uuid id) -> std::shared_ptr<Attribute>
{
  auto sa = std::make_shared<Attribute>(id);
  sa->setName(QString::fromStdString(name));
  auto formula = std::make_shared<ScaledSumDerivationFormula>(coefficients, divisor);
  sa->setFormula(formula.get());
  for (auto const& pid : parentIds) {
    auto ref = std::make_shared<ParentRef>();
    ref->setTargetIdString(QString::fromStdString(boost::uuids::to_string(pid)));
    // Insert ParentRef after derivation formula (position 0), before any direct formula
    int parentPos = 1; // Count existing ParentRefs
    for (int i = 1; i < sa->size(); ++i) {
      if (dynamic_cast<ParentRef*>(sa->childAt(i).get()) != nullptr) {
        parentPos = i + 1;
      }
    }
    sa->insertChild(parentPos, ref);
  }
  return sa;
}

static auto makeGurps4Setup() -> Gurps4Setup
{
  Gurps4Setup s;

  // ── Basic Attributes ──────────────────────────────────────────────────────
  s.st = makeBasicAttr("ST", 10, ids::st());
  s.dx = makeBasicAttr("DX", 20, ids::dx());
  s.iq = makeBasicAttr("IQ", 20, ids::iq());
  s.ht = makeBasicAttr("HT", 10, ids::ht());

  // ── Single-parent scaled-sum secondaries ──────────────────────────────────
  s.hp = makeScaledSecondary("HP", {1}, 1, {ids::st()}, ids::hp());
  s.will = makeScaledSecondary("Will", {1}, 1, {ids::iq()}, ids::will());
  s.per = makeScaledSecondary("Per", {1}, 1, {ids::iq()}, ids::per());
  s.fp = makeScaledSecondary("FP", {1}, 1, {ids::ht()}, ids::fp());

  // ── BasicSpeed = (DX + HT) / 4 ───────────────────────────────────────────
  s.basicSpeed =
      makeScaledSecondary("Basic Speed", {1, 1}, 4, {ids::dx(), ids::ht()}, ids::basicSpeed());

  // ── BasicMove ≈ BasicSpeed / 1 ────────────────────────────────────────────
  s.basicMove = makeScaledSecondary("Basic Move", {1}, 1, {ids::basicSpeed()}, ids::basicMove());

  // ── BasicLift = ST² / 5 ───────────────────────────────────────────────────
  {
    auto sa = std::make_shared<Attribute>(ids::basicLift());
    sa->setName("Basic Lift");
    auto formula = std::make_shared<QuadraticDerivationFormula>(5);
    sa->setFormula(formula.get());
    auto ref = std::make_shared<ParentRef>();
    ref->setTargetIdString(QString::fromStdString(boost::uuids::to_string(ids::st())));
    sa->insertChild(1, ref); // Insert ParentRef after derivation formula
    s.basicLift = sa;
  }

  // ── Thrust Damage = lookup from ST ────────────────────────────────────────
  {
    auto sa = std::make_shared<Attribute>(ids::thrustDmg());
    sa->setName("Thrust Damage");
    auto formula = std::make_shared<LookupDerivationFormula>(k_thrustTable);
    sa->setFormula(formula.get());
    auto ref = std::make_shared<ParentRef>();
    ref->setTargetIdString(QString::fromStdString(boost::uuids::to_string(ids::st())));
    sa->insertChild(1, ref); // Insert ParentRef after derivation formula
    s.thrustDmg = sa;
  }

  // ── Swing Damage = lookup from ST ─────────────────────────────────────────
  {
    auto sa = std::make_shared<Attribute>(ids::swingDmg());
    sa->setName("Swing Damage");
    auto formula = std::make_shared<LookupDerivationFormula>(k_swingTable);
    sa->setFormula(formula.get());
    auto ref = std::make_shared<ParentRef>();
    ref->setTargetIdString(QString::fromStdString(boost::uuids::to_string(ids::st())));
    sa->insertChild(1, ref); // Insert ParentRef after derivation formula
    s.swingDmg = sa;
  }

  return s;
}

// ── Round-trip helper ─────────────────────────────────────────────────────────

template <class S>
static auto roundTrip(std::vector<std::shared_ptr<infrastructure::TreeItem>> const& items,
                      infrastructure::ObjectFactory& factory)
    -> std::vector<std::shared_ptr<infrastructure::TreeItem>>
{
  S ser;
  std::ostringstream out;
  ser.write(out, items);

  std::istringstream in{out.str()};
  Serializer::FileHeader header{};
  in.read(reinterpret_cast<char*>(header.data()), Serializer::kHeaderSize);
  auto const version =
      static_cast<uint16_t>((static_cast<uint16_t>(std::to_integer<uint8_t>(header[21])) << 8) |
                            static_cast<uint16_t>(std::to_integer<uint8_t>(header[22])));

  infrastructure::ObjectRegistry reg;
  return ser.read(header, version, in, factory, reg);
}

// ── Search helpers ────────────────────────────────────────────────────────────

static auto findById(std::vector<std::shared_ptr<infrastructure::TreeItem>> const& items,
                     boost::uuids::uuid const& id) -> infrastructure::TreeItem*
{
  for (auto const& item : items) {
    if (item->objectId() == id) {
      return item.get();
    }
  }
  return nullptr;
}

static auto costPerLevelOf(Attribute const& attr) -> int
{
  auto* formula = attr.formula();
  if (!formula) {
    return 0;
  }
  auto* f = dynamic_cast<LinearFormula const*>(formula);
  return f ? f->costPerLevel() : 0;
}

static auto derivationTypeOf(Attribute const& sa) -> boost::uuids::uuid
{
  auto* df = sa.formula();
  return df ? df->typeId() : boost::uuids::uuid{};
}

static auto parentIdStringOf(Attribute const& sa, int i) -> QString
{
  // ParentRef access through TreeItem hierarchy
  if (i < 0 || i >= sa.size()) {
    return QString{};
  }

  // Skip formula if present (both direct Formula and DerivationFormula inherit from Formula)
  int offset = 0;
  if (sa.size() > 0 && dynamic_cast<Formula const*>(sa.childAt(0).get())) {
    offset = 1;
  }

  // ParentRefs come after formula
  int parentIdx = offset + i;
  if (parentIdx >= sa.size()) {
    return QString{};
  }

  auto* ref = dynamic_cast<ParentRef const*>(sa.childAt(parentIdx).get());
  return ref ? ref->targetIdString() : QString{};
}

// Helper to count ParentRef children (replaces parentCount() accessor)
static auto countParents(Attribute const& sa) -> int
{
  int count = 0;
  for (int i = 0; i < sa.size(); ++i) {
    if (dynamic_cast<ParentRef const*>(sa.childAt(i).get()) != nullptr) {
      ++count;
    }
  }
  return count;
}

// ── Typed fixture ─────────────────────────────────────────────────────────────

template <class S>
class Gurps4RoundTripTest : public ::testing::Test
{
protected:
  infrastructure::ObjectFactory factory;
  Gurps4Setup setup;
  std::vector<std::shared_ptr<infrastructure::TreeItem>> rt; // result of roundTrip

  auto SetUp() -> void override
  {
    registerSystemObjects(factory);
    setup = makeGurps4Setup();
    rt = roundTrip<S>(setup.items(), factory);
  }
};

using Gurps4Serializers = ::testing::Types<JsonSerializer, XmlSerializer>;
TYPED_TEST_SUITE(Gurps4RoundTripTest, Gurps4Serializers);

// ── Item count ────────────────────────────────────────────────────────────────

TYPED_TEST(Gurps4RoundTripTest, PreservesAllThirteenItems) { EXPECT_EQ(this->rt.size(), 13u); }

// ── Names ─────────────────────────────────────────────────────────────────────

TYPED_TEST(Gurps4RoundTripTest, PreservesBasicAttributeNames)
{
  auto const expectName = [&](boost::uuids::uuid id, QString const& name) {
    auto* item = findById(this->rt, id);
    ASSERT_NE(item, nullptr) << "item not found";
    auto* bo = dynamic_cast<BaseObject*>(item);
    ASSERT_NE(bo, nullptr);
    EXPECT_EQ(bo->name(), name);
  };
  expectName(ids::st(), "ST");
  expectName(ids::dx(), "DX");
  expectName(ids::iq(), "IQ");
  expectName(ids::ht(), "HT");
}

TYPED_TEST(Gurps4RoundTripTest, PreservesDerivedAttributeNames)
{
  auto const expectName = [&](boost::uuids::uuid id, QString const& name) {
    auto* item = findById(this->rt, id);
    ASSERT_NE(item, nullptr) << "item not found";
    auto* sa = dynamic_cast<Attribute*>(item);
    ASSERT_NE(sa, nullptr);
    EXPECT_EQ(sa->name(), name);
  };
  expectName(ids::hp(), "HP");
  expectName(ids::will(), "Will");
  expectName(ids::per(), "Per");
  expectName(ids::fp(), "FP");
  expectName(ids::basicSpeed(), "Basic Speed");
  expectName(ids::basicMove(), "Basic Move");
  expectName(ids::basicLift(), "Basic Lift");
  expectName(ids::thrustDmg(), "Thrust Damage");
  expectName(ids::swingDmg(), "Swing Damage");
}

// ── Basic attribute formulas ──────────────────────────────────────────────────

TYPED_TEST(Gurps4RoundTripTest, BasicAttributesHaveLinearFormulas)
{
  for (auto const& id : {ids::st(), ids::dx(), ids::iq(), ids::ht()}) {
    auto* item = findById(this->rt, id);
    ASSERT_NE(item, nullptr);
    auto* attr = dynamic_cast<Attribute*>(item);
    ASSERT_NE(attr, nullptr) << "expected Attribute";
    ASSERT_EQ(attr->size(), 1) << "expected exactly one formula child";
    EXPECT_NE(dynamic_cast<LinearFormula*>(attr->childAt(0).get()), nullptr)
        << "expected LinearFormula child";
  }
}

TYPED_TEST(Gurps4RoundTripTest, STAndHTCostTenCpPerLevel)
{
  auto* stItem = dynamic_cast<Attribute*>(findById(this->rt, ids::st()));
  auto* htItem = dynamic_cast<Attribute*>(findById(this->rt, ids::ht()));
  ASSERT_NE(stItem, nullptr);
  ASSERT_NE(htItem, nullptr);
  EXPECT_EQ(costPerLevelOf(*stItem), 10);
  EXPECT_EQ(costPerLevelOf(*htItem), 10);
}

TYPED_TEST(Gurps4RoundTripTest, DXAndIQCostTwentyCpPerLevel)
{
  auto* dxItem = dynamic_cast<Attribute*>(findById(this->rt, ids::dx()));
  auto* iqItem = dynamic_cast<Attribute*>(findById(this->rt, ids::iq()));
  ASSERT_NE(dxItem, nullptr);
  ASSERT_NE(iqItem, nullptr);
  EXPECT_EQ(costPerLevelOf(*dxItem), 20);
  EXPECT_EQ(costPerLevelOf(*iqItem), 20);
}

// ── Secondary attribute derivation formulas ───────────────────────────────────

TYPED_TEST(Gurps4RoundTripTest, SingleParentSecondariesUseScaledSumDivisorOne)
{
  for (auto const& id : {ids::hp(), ids::will(), ids::per(), ids::fp(), ids::basicMove()}) {
    auto* sa = dynamic_cast<Attribute*>(findById(this->rt, id));
    ASSERT_NE(sa, nullptr);
    EXPECT_EQ(derivationTypeOf(*sa), ScaledSumDerivationFormula::classId());
    auto* df = dynamic_cast<ScaledSumDerivationFormula*>(sa->formula());
    ASSERT_NE(df, nullptr);
    EXPECT_EQ(df->divisor(), 1);
    EXPECT_EQ(df->coefficients(), (QList<int>{1}));
  }
}

TYPED_TEST(Gurps4RoundTripTest, BasicSpeedUsesTwoParentsAndDivisorFour)
{
  auto* sa = dynamic_cast<Attribute*>(findById(this->rt, ids::basicSpeed()));
  ASSERT_NE(sa, nullptr);
  EXPECT_EQ(derivationTypeOf(*sa), ScaledSumDerivationFormula::classId());
  auto* df = dynamic_cast<ScaledSumDerivationFormula*>(sa->formula());
  ASSERT_NE(df, nullptr);
  EXPECT_EQ(df->divisor(), 4);
  EXPECT_EQ(df->coefficients(), (QList<int>{1, 1}));
  EXPECT_EQ(countParents(*sa), 2);
}

TYPED_TEST(Gurps4RoundTripTest, BasicLiftUsesQuadraticDivisorFive)
{
  auto* sa = dynamic_cast<Attribute*>(findById(this->rt, ids::basicLift()));
  ASSERT_NE(sa, nullptr);
  EXPECT_EQ(derivationTypeOf(*sa), QuadraticDerivationFormula::classId());
  auto* df = dynamic_cast<QuadraticDerivationFormula*>(sa->formula());
  ASSERT_NE(df, nullptr);
  EXPECT_EQ(df->divisor(), 5);
}

TYPED_TEST(Gurps4RoundTripTest, DamageLookupTablesHaveTwentyEntries)
{
  for (auto const& id : {ids::thrustDmg(), ids::swingDmg()}) {
    auto* sa = dynamic_cast<Attribute*>(findById(this->rt, id));
    ASSERT_NE(sa, nullptr);
    EXPECT_EQ(derivationTypeOf(*sa), LookupDerivationFormula::classId());
    auto* df = dynamic_cast<LookupDerivationFormula*>(sa->formula());
    ASSERT_NE(df, nullptr);
    EXPECT_EQ(static_cast<int>(df->table().size()), 20);
  }
}

TYPED_TEST(Gurps4RoundTripTest, ThrustDamageTableSpotChecks)
{
  auto* sa = dynamic_cast<Attribute*>(findById(this->rt, ids::thrustDmg()));
  ASSERT_NE(sa, nullptr);
  auto* df = dynamic_cast<LookupDerivationFormula*>(sa->formula());
  ASSERT_NE(df, nullptr);
  auto const& table = df->table();
  EXPECT_EQ(table.at(1), -5); // ST 1 thrust
  EXPECT_EQ(table.at(10), 0); // ST 10 thrust
  EXPECT_EQ(table.at(20), 5); // ST 20 thrust
}

TYPED_TEST(Gurps4RoundTripTest, SwingDamageTableSpotChecks)
{
  auto* sa = dynamic_cast<Attribute*>(findById(this->rt, ids::swingDmg()));
  ASSERT_NE(sa, nullptr);
  auto* df = dynamic_cast<LookupDerivationFormula*>(sa->formula());
  ASSERT_NE(df, nullptr);
  auto const& table = df->table();
  EXPECT_EQ(table.at(1), -3); // ST 1 swing
  EXPECT_EQ(table.at(10), 2); // ST 10 swing
  EXPECT_EQ(table.at(20), 8); // ST 20 swing
}

// ── Parent references ─────────────────────────────────────────────────────────

TYPED_TEST(Gurps4RoundTripTest, HPParentPointsToST)
{
  auto* sa = dynamic_cast<Attribute*>(findById(this->rt, ids::hp()));
  ASSERT_NE(sa, nullptr);
  ASSERT_EQ(countParents(*sa), 1);
  EXPECT_EQ(parentIdStringOf(*sa, 0), QString::fromStdString(boost::uuids::to_string(ids::st())));
}

TYPED_TEST(Gurps4RoundTripTest, WillParentPointsToIQ)
{
  auto* sa = dynamic_cast<Attribute*>(findById(this->rt, ids::will()));
  ASSERT_NE(sa, nullptr);
  ASSERT_EQ(countParents(*sa), 1);
  EXPECT_EQ(parentIdStringOf(*sa, 0), QString::fromStdString(boost::uuids::to_string(ids::iq())));
}

TYPED_TEST(Gurps4RoundTripTest, PerParentPointsToIQ)
{
  auto* sa = dynamic_cast<Attribute*>(findById(this->rt, ids::per()));
  ASSERT_NE(sa, nullptr);
  ASSERT_EQ(countParents(*sa), 1);
  EXPECT_EQ(parentIdStringOf(*sa, 0), QString::fromStdString(boost::uuids::to_string(ids::iq())));
}

TYPED_TEST(Gurps4RoundTripTest, FPParentPointsToHT)
{
  auto* sa = dynamic_cast<Attribute*>(findById(this->rt, ids::fp()));
  ASSERT_NE(sa, nullptr);
  ASSERT_EQ(countParents(*sa), 1);
  EXPECT_EQ(parentIdStringOf(*sa, 0), QString::fromStdString(boost::uuids::to_string(ids::ht())));
}

TYPED_TEST(Gurps4RoundTripTest, BasicSpeedParentsAreDXAndHT)
{
  auto* sa = dynamic_cast<Attribute*>(findById(this->rt, ids::basicSpeed()));
  ASSERT_NE(sa, nullptr);
  ASSERT_EQ(countParents(*sa), 2);
  EXPECT_EQ(parentIdStringOf(*sa, 0), QString::fromStdString(boost::uuids::to_string(ids::dx())));
  EXPECT_EQ(parentIdStringOf(*sa, 1), QString::fromStdString(boost::uuids::to_string(ids::ht())));
}

TYPED_TEST(Gurps4RoundTripTest, BasicMoveParentPointsToBasicSpeed)
{
  auto* sa = dynamic_cast<Attribute*>(findById(this->rt, ids::basicMove()));
  ASSERT_NE(sa, nullptr);
  ASSERT_EQ(countParents(*sa), 1);
  EXPECT_EQ(parentIdStringOf(*sa, 0),
            QString::fromStdString(boost::uuids::to_string(ids::basicSpeed())));
}

TYPED_TEST(Gurps4RoundTripTest, BasicLiftParentPointsToST)
{
  auto* sa = dynamic_cast<Attribute*>(findById(this->rt, ids::basicLift()));
  ASSERT_NE(sa, nullptr);
  ASSERT_EQ(countParents(*sa), 1);
  EXPECT_EQ(parentIdStringOf(*sa, 0), QString::fromStdString(boost::uuids::to_string(ids::st())));
}

TYPED_TEST(Gurps4RoundTripTest, ThrustDmgParentPointsToST)
{
  auto* sa = dynamic_cast<Attribute*>(findById(this->rt, ids::thrustDmg()));
  ASSERT_NE(sa, nullptr);
  ASSERT_EQ(countParents(*sa), 1);
  EXPECT_EQ(parentIdStringOf(*sa, 0), QString::fromStdString(boost::uuids::to_string(ids::st())));
}

TYPED_TEST(Gurps4RoundTripTest, SwingDmgParentPointsToST)
{
  auto* sa = dynamic_cast<Attribute*>(findById(this->rt, ids::swingDmg()));
  ASSERT_NE(sa, nullptr);
  ASSERT_EQ(countParents(*sa), 1);
  EXPECT_EQ(parentIdStringOf(*sa, 0), QString::fromStdString(boost::uuids::to_string(ids::st())));
}

// ── IQ is shared parent of both Will and Per ─────────────────────────────────

TYPED_TEST(Gurps4RoundTripTest, WillAndPerShareSameIQParentId)
{
  auto* willSa = dynamic_cast<Attribute*>(findById(this->rt, ids::will()));
  auto* perSa = dynamic_cast<Attribute*>(findById(this->rt, ids::per()));
  ASSERT_NE(willSa, nullptr);
  ASSERT_NE(perSa, nullptr);
  EXPECT_EQ(parentIdStringOf(*willSa, 0), parentIdStringOf(*perSa, 0));
}
