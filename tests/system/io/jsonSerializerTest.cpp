#include <jsonSerializer.hpp>

#include <objectFactory.hpp>
#include <objectRegistry.hpp>
#include <testTypes.hpp>
#include <treeItem.hpp>

#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid.hpp>

#include <QCoreApplication>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using testing::SizeIs;
using namespace std::chrono_literals;
// QCoreApplication is provided by ioTest.cpp (same test binary).

namespace {
static void processEvents() { QCoreApplication::processEvents(QEventLoop::AllEvents); }
} // namespace

namespace {

// ── Helpers ───────────────────────────────────────────────────────────────────

auto readAll(gurps_system::JsonSerializer const& s, std::string const& data,
             infrastructure::ObjectFactory& factory, infrastructure::ObjectRegistry& registry)
    -> std::vector<std::shared_ptr<infrastructure::TreeItem>>
{
  std::istringstream stream{data};
  gurps_system::Serializer::FileHeader header{};
  if (!stream.read(reinterpret_cast<char*>(header.data()), gurps_system::Serializer::kHeaderSize)) {
    return {};
  }
  auto const version =
      static_cast<uint16_t>((static_cast<uint16_t>(std::to_integer<uint8_t>(header[21])) << 8) |
                            static_cast<uint16_t>(std::to_integer<uint8_t>(header[22])));
  return s.read(header, version, stream, factory, registry);
}

auto findByType(std::vector<std::shared_ptr<infrastructure::TreeItem>> const& items,
                boost::uuids::uuid const& typeId) -> std::shared_ptr<infrastructure::TreeItem>
{
  for (auto const& item : items) {
    if (item->typeId() == typeId) {
      return item;
    }
  }
  return nullptr;
}

} // namespace

// ── Fixture ───────────────────────────────────────────────────────────────────

class JsonSerializerTest : public ::testing::Test
{
protected:
  infrastructure::ObjectFactory factory;
  infrastructure::ObjectRegistry registry;
  gurps_system::JsonSerializer serializer;

  JsonSerializerTest()
  {
    factory.install(AlphaItem::classId(),
                    [](boost::uuids::uuid id) { return std::make_shared<AlphaItem>(id); });
    factory.install(BetaItem::classId(),
                    [](boost::uuids::uuid id) { return std::make_shared<BetaItem>(id); });
    factory.install(GammaItem::classId(),
                    [](boost::uuids::uuid id) { return std::make_shared<GammaItem>(id); });
    factory.install(DeltaItem::classId(),
                    [](boost::uuids::uuid id) { return std::make_shared<DeltaItem>(id); });
    factory.install(EpsilonItem::classId(),
                    [](boost::uuids::uuid id) { return std::make_shared<EpsilonItem>(id); });
    factory.install(ZetaItem::classId(),
                    [](boost::uuids::uuid id) { return std::make_shared<ZetaItem>(id); });
  }
};

// ── Capabilities ──────────────────────────────────────────────────────────────

TEST_F(JsonSerializerTest, IdIsStable)
{
  EXPECT_EQ(serializer.id(), gurps_system::JsonSerializer::classId());
}

TEST_F(JsonSerializerTest, FiltersContainJsonEntries)
{
  EXPECT_TRUE(serializer.filters().contains("GURPS JSON (*.json)"));
  EXPECT_TRUE(serializer.filters().contains("*.json"));
}

TEST_F(JsonSerializerTest, ProvidesKnownFilter)
{
  EXPECT_TRUE(serializer.provides("*.json"));
  EXPECT_FALSE(serializer.provides("*.xml"));
}

// ── Write ─────────────────────────────────────────────────────────────────────

TEST_F(JsonSerializerTest, WriteEmptyRootsHasMagicHeader)
{
  std::ostringstream out;
  serializer.write(out, {});
  auto const& s = out.str();
  ASSERT_GE(s.size(), 5u);
  EXPECT_EQ(s.substr(0, 5), "DMFMT");
}

TEST_F(JsonSerializerTest, WriteSingleItemProducesNonEmptyOutput)
{
  auto item = std::make_shared<AlphaItem>();
  std::ostringstream out;
  serializer.write(out, {item});
  EXPECT_FALSE(out.str().empty());
}

TEST_F(JsonSerializerTest, WriteSingleItemContainsTypeId)
{
  auto item = std::make_shared<AlphaItem>();
  std::ostringstream out;
  serializer.write(out, {item});
  EXPECT_NE(out.str().find("aaaaaaaa-0000-4000-8000-000000000001"), std::string::npos);
}

TEST_F(JsonSerializerTest, WriteSingleItemContainsObjectId)
{
  auto const id = boost::uuids::string_generator{}("11111111-0000-4000-8000-000000000099");
  auto item = std::make_shared<AlphaItem>(id);
  std::ostringstream out;
  serializer.write(out, {item});
  EXPECT_NE(out.str().find("11111111-0000-4000-8000-000000000099"), std::string::npos);
}

TEST_F(JsonSerializerTest, WriteTreeIncludesChildInFlatList)
{
  // root(Alpha) → child(Beta): both must appear in the output
  auto root = std::make_shared<AlphaItem>();
  auto child = std::make_shared<BetaItem>();
  root->insertChild(0, child);

  std::ostringstream out;
  serializer.write(out, {root});

  EXPECT_NE(out.str().find("aaaaaaaa-0000-4000-8000-000000000001"), std::string::npos);
  EXPECT_NE(out.str().find("bbbbbbbb-0000-4000-8000-000000000002"), std::string::npos);
}

TEST_F(JsonSerializerTest, WriteDeduplicatesSharedChild)
{
  // Two Alpha roots sharing one Beta child → output must contain Beta only once
  auto child = std::make_shared<BetaItem>();
  auto root1 = std::make_shared<AlphaItem>();
  auto root2 = std::make_shared<AlphaItem>();
  root1->insertChild(0, child);
  root2->insertChild(0, child);

  std::ostringstream out;
  serializer.write(out, {root1, root2});

  // Count occurrences of BetaItem's typeId UUID in the output
  auto const& str = out.str();
  auto const typeStr = std::string{"bbbbbbbb-0000-4000-8000-000000000002"};
  std::size_t count = 0;
  std::size_t pos = 0;
  while ((pos = str.find(typeStr, pos)) != std::string::npos) {
    ++count;
    ++pos;
  }
  EXPECT_EQ(count, 1u);
}

TEST_F(JsonSerializerTest, WriteSerializesStringProperty)
{
  auto item = std::make_shared<AlphaItem>();
  item->setName("Alice");
  std::ostringstream out;
  serializer.write(out, {item});
  EXPECT_NE(out.str().find("Alice"), std::string::npos);
  EXPECT_NE(out.str().find("QString"), std::string::npos);
}

TEST_F(JsonSerializerTest, WriteSerializesIntProperty)
{
  auto item = std::make_shared<AlphaItem>();
  item->setLevel(42);
  std::ostringstream out;
  serializer.write(out, {item});
  EXPECT_NE(out.str().find("42"), std::string::npos);
  EXPECT_NE(out.str().find("int"), std::string::npos);
}

TEST_F(JsonSerializerTest, WriteSerializesStringListProperty)
{
  auto item = std::make_shared<BetaItem>();
  item->setTags({"warrior", "ranger"});
  std::ostringstream out;
  serializer.write(out, {item});
  EXPECT_NE(out.str().find("warrior"), std::string::npos);
  EXPECT_NE(out.str().find("ranger"), std::string::npos);
  EXPECT_NE(out.str().find("QStringList"), std::string::npos);
}

// ── Read ──────────────────────────────────────────────────────────────────────

TEST_F(JsonSerializerTest, ReadEmptyArrayReturnsEmpty)
{
  std::istringstream stream{"[\n]"};
  gurps_system::Serializer::FileHeader header{};
  EXPECT_TRUE(serializer.read(header, 1, stream, factory, registry).empty());
}

TEST_F(JsonSerializerTest, ReadMalformedJsonReturnsEmpty)
{
  std::istringstream stream{"not valid json"};
  gurps_system::Serializer::FileHeader header{};
  EXPECT_TRUE(serializer.read(header, 1, stream, factory, registry).empty());
}

TEST_F(JsonSerializerTest, ReadJsonObjectRootReturnsEmpty)
{
  std::istringstream stream{R"({"key": 1})"};
  gurps_system::Serializer::FileHeader header{};
  EXPECT_TRUE(serializer.read(header, 1, stream, factory, registry).empty());
}

TEST_F(JsonSerializerTest, ReadSingleItemReturnsOneObject)
{
  auto const json = R"([
    { "typeId": "aaaaaaaa-0000-4000-8000-000000000001",
      "objectId": "cccccccc-0000-4000-8000-000000000003", 
      "root": true }
  ])";
  std::istringstream stream{json};
  gurps_system::Serializer::FileHeader header{};
  auto items = serializer.read(header, 1, stream, factory, registry);
  EXPECT_EQ(items.size(), 1u);
}

TEST_F(JsonSerializerTest, ReadRestoresCorrectTypeId)
{
  auto const json = R"([
    { "typeId": "aaaaaaaa-0000-4000-8000-000000000001",
      "objectId": "cccccccc-0000-4000-8000-000000000003", 
      "root": true }
  ])";
  std::istringstream stream{json};
  gurps_system::Serializer::FileHeader header{};
  auto items = serializer.read(header, 1, stream, factory, registry);
  ASSERT_EQ(items.size(), 1u);
  EXPECT_EQ(items[0]->typeId(), AlphaItem::classId());
}

TEST_F(JsonSerializerTest, ReadRestoresCorrectObjectId)
{
  auto const expected = boost::uuids::string_generator{}("cccccccc-0000-4000-8000-000000000003");
  auto const json = R"([
    { "typeId": "aaaaaaaa-0000-4000-8000-000000000001",
      "objectId": "cccccccc-0000-4000-8000-000000000003", 
      "root": true }
  ])";
  std::istringstream stream{json};
  gurps_system::Serializer::FileHeader header{};
  auto items = serializer.read(header, 1, stream, factory, registry);
  ASSERT_EQ(items.size(), 1u);
  EXPECT_EQ(items[0]->objectId(), expected);
}

TEST_F(JsonSerializerTest, ReadRegistersItemInRegistry)
{
  auto const expected = boost::uuids::string_generator{}("cccccccc-0000-4000-8000-000000000003");
  auto const json = R"([
    { "typeId": "aaaaaaaa-0000-4000-8000-000000000001",
      "objectId": "cccccccc-0000-4000-8000-000000000003", 
      "root": true }
  ])";
  std::istringstream stream{json};
  gurps_system::Serializer::FileHeader header{};
  auto items = serializer.read(header, 1, stream, factory, registry); // keep shared_ptrs alive
  EXPECT_FALSE(registry.findObject(expected).expired());
}

TEST_F(JsonSerializerTest, ReadSkipsEntryWithUnknownTypeId)
{
  // Unknown typeId: factory can't create it → entry silently skipped
  auto const json = R"([
    { "typeId": "00000000-0000-4000-8000-000000000000",
      "objectId": "cccccccc-0000-4000-8000-000000000003", 
      "root": true }
  ])";
  std::istringstream stream{json};
  gurps_system::Serializer::FileHeader header{};
  auto items = serializer.read(header, 1, stream, factory, registry);
  EXPECT_TRUE(items.empty());
}

TEST_F(JsonSerializerTest, ReadTwoItemsReturnsBoth)
{
  auto const json = R"([
    { "typeId": "aaaaaaaa-0000-4000-8000-000000000001",
      "objectId": "cccccccc-0000-4000-8000-000000000003", 
      "root": true },
    { "typeId": "bbbbbbbb-0000-4000-8000-000000000002",
      "objectId": "dddddddd-0000-4000-8000-000000000004", 
      "root": true }
  ])";
  std::istringstream stream{json};
  gurps_system::Serializer::FileHeader header{};
  auto items = serializer.read(header, 1, stream, factory, registry);
  EXPECT_EQ(items.size(), 2u);
  EXPECT_NE(findByType(items, AlphaItem::classId()), nullptr);
  EXPECT_NE(findByType(items, BetaItem::classId()), nullptr);
}

// ── Round-trip ────────────────────────────────────────────────────────────────

TEST_F(JsonSerializerTest, RoundTripPreservesTypeId)
{
  auto original = std::make_shared<AlphaItem>();
  std::ostringstream out;
  serializer.write(out, {original});

  infrastructure::ObjectRegistry reg2;
  auto items = readAll(serializer, out.str(), factory, reg2);
  ASSERT_EQ(items.size(), 1u);
  EXPECT_EQ(items[0]->typeId(), AlphaItem::classId());
}

TEST_F(JsonSerializerTest, RoundTripPreservesObjectId)
{
  auto original = std::make_shared<AlphaItem>();
  auto const originalId = original->objectId();
  std::ostringstream out;
  serializer.write(out, {original});

  infrastructure::ObjectRegistry reg2;
  auto items = readAll(serializer, out.str(), factory, reg2);
  ASSERT_EQ(items.size(), 1u);
  EXPECT_EQ(items[0]->objectId(), originalId);
}

TEST_F(JsonSerializerTest, RoundTripRestoresTreeStructure)
{
  // Write root → child; read back yields 1 root with 1 child already wired.
  auto root = std::make_shared<AlphaItem>();
  auto child = std::make_shared<BetaItem>();
  root->insertChild(0, child);

  std::ostringstream out;
  serializer.write(out, {root});

  infrastructure::ObjectRegistry reg2;
  auto roots = readAll(serializer, out.str(), factory, reg2);
  ASSERT_THAT(roots, SizeIs(1));
  EXPECT_EQ(roots[0]->typeId(), AlphaItem::classId());
  ASSERT_THAT(*roots[0], SizeIs(1));
  EXPECT_EQ(roots[0]->childAt(0)->typeId(), BetaItem::classId());
  EXPECT_EQ(roots[0]->childAt(0)->objectId(), child->objectId());
}

TEST_F(JsonSerializerTest, RoundTripSharedChildBelongsToFirstParent)
{
  // A shared child is claimed in the file by the first DFS-visited parent.
  // After round-trip both roots are returned; the child lives under exactly
  // one of them (total child count across both roots == 1).
  auto shared = std::make_shared<BetaItem>();
  auto root1 = std::make_shared<AlphaItem>();
  auto root2 = std::make_shared<AlphaItem>();
  root1->insertChild(0, shared);
  root2->insertChild(0, shared);

  std::ostringstream out;
  serializer.write(out, {root1, root2});

  infrastructure::ObjectRegistry reg2;
  auto roots = readAll(serializer, out.str(), factory, reg2);
  ASSERT_THAT(roots, SizeIs(2));
  ASSERT_THAT(*roots.at(0), SizeIs(1));
  ASSERT_THAT(*roots.at(1), SizeIs(1));
  EXPECT_EQ(roots[0]->childAt(0), roots[1]->childAt(0));
}

TEST_F(JsonSerializerTest, RoundTripSharedChildObjectIdIsPreserved)
{
  auto shared = std::make_shared<BetaItem>();
  auto const sharedId = shared->objectId();
  auto root1 = std::make_shared<AlphaItem>();
  auto root2 = std::make_shared<AlphaItem>();
  root1->insertChild(0, shared);
  root2->insertChild(0, shared);

  std::ostringstream out;
  serializer.write(out, {root1, root2});

  infrastructure::ObjectRegistry reg2;
  auto roots = readAll(serializer, out.str(), factory, reg2);
  ASSERT_EQ(roots.size(), 2u);

  // Find the root that ended up with the child
  std::shared_ptr<infrastructure::TreeItem> foundChild;
  for (auto const& root : roots) {
    if (root->size() > 0) {
      foundChild = root->childAt(0);
      break;
    }
  }
  ASSERT_NE(foundChild, nullptr);
  EXPECT_EQ(foundChild->objectId(), sharedId);
  EXPECT_EQ(foundChild->typeId(), BetaItem::classId());
}

// ── Property round-trips ────────────────────────────────────────────────────────────────

TEST_F(JsonSerializerTest, RoundTripPreservesStringProperty)
{
  auto original = std::make_shared<AlphaItem>();
  original->setName("Aragorn");
  std::ostringstream out;
  serializer.write(out, {original});

  infrastructure::ObjectRegistry reg2;
  auto items = readAll(serializer, out.str(), factory, reg2);
  ASSERT_EQ(items.size(), 1u);
  auto* restored = qobject_cast<AlphaItem*>(items[0].get());
  ASSERT_NE(restored, nullptr);
  EXPECT_EQ(restored->name(), "Aragorn");
}

TEST_F(JsonSerializerTest, RoundTripPreservesIntProperty)
{
  auto original = std::make_shared<AlphaItem>();
  original->setLevel(7);
  std::ostringstream out;
  serializer.write(out, {original});

  infrastructure::ObjectRegistry reg2;
  auto items = readAll(serializer, out.str(), factory, reg2);
  ASSERT_EQ(items.size(), 1u);
  auto* restored = qobject_cast<AlphaItem*>(items[0].get());
  ASSERT_NE(restored, nullptr);
  EXPECT_EQ(restored->level(), 7);
}

TEST_F(JsonSerializerTest, RoundTripPreservesStringListProperty)
{
  auto original = std::make_shared<BetaItem>();
  original->setTags({"fighter", "mage"});
  std::ostringstream out;
  serializer.write(out, {original});

  infrastructure::ObjectRegistry reg2;
  auto items = readAll(serializer, out.str(), factory, reg2);
  ASSERT_EQ(items.size(), 1u);
  auto* restored = qobject_cast<BetaItem*>(items[0].get());
  ASSERT_NE(restored, nullptr);
  EXPECT_EQ(restored->tags(), QStringList({"fighter", "mage"}));
}

TEST_F(JsonSerializerTest, RoundTripPreservesInheritedAndOwnProperties)
{
  auto original = std::make_shared<GammaItem>();
  original->setName("Legolas");
  original->setLevel(12);
  original->setTitle("Elven Lord");

  std::ostringstream out;
  serializer.write(out, {original});

  infrastructure::ObjectRegistry reg2;
  auto items = readAll(serializer, out.str(), factory, reg2);
  ASSERT_EQ(items.size(), 1u);
  auto* restored = qobject_cast<GammaItem*>(items[0].get());
  ASSERT_NE(restored, nullptr);
  EXPECT_EQ(restored->name(), "Legolas");
  EXPECT_EQ(restored->level(), 12);
  EXPECT_EQ(restored->title(), "Elven Lord");
}

TEST_F(JsonSerializerTest, WriteThrowsForUnsupportedPropertyType)
{
  auto item = std::make_shared<DeltaItem>();
  item->setScore(DeltaItem::ItemType::Beta);
  std::ostringstream out;
  EXPECT_THROW(serializer.write(out, {item}), std::invalid_argument);
}

TEST_F(JsonSerializerTest, ReadThrowsForUnsupportedPropertyType)
{
  auto const headerBytes =
      gurps_system::Serializer::buildHeader(gurps_system::JsonSerializer::classId(), 1);
  std::string data(reinterpret_cast<char const*>(headerBytes.data()),
                   gurps_system::Serializer::kHeaderSize);
  // Craft a JSON entry with a "double" property, which has no registered codec.
  data += R"([{"typeId":"dddddddd-0000-4000-8000-000000000004",)"
          R"("objectId":"eeeeeeee-0000-4000-8000-000000000005",)"
          R"("properties":{"score":{"type":"NoneExistingType","value":"Alpha"}}}])";

  infrastructure::ObjectRegistry reg2;
  EXPECT_THROW(readAll(serializer, data, factory, reg2), std::invalid_argument);
}

TEST_F(JsonSerializerTest, RoundTripSharedChildAlsoTopLevel)
{
  // shared is a child of both root1 and root2, AND explicitly a top-level root.
  // After round-trip: 3 roots returned; both alpha roots have shared as child
  // (same object pointer), and shared is itself one of the root objects.
  auto shared = std::make_shared<BetaItem>();
  auto const sharedId = shared->objectId();
  auto root1 = std::make_shared<AlphaItem>();
  auto root2 = std::make_shared<AlphaItem>();
  root1->insertChild(0, shared);
  root2->insertChild(0, shared);

  std::ostringstream out;
  serializer.write(out, {root1, root2, shared});

  infrastructure::ObjectRegistry reg2;
  auto roots = readAll(serializer, out.str(), factory, reg2);
  ASSERT_THAT(roots, SizeIs(3));

  ASSERT_THAT(*roots.at(0), SizeIs(0));
  EXPECT_EQ(roots.at(0)->objectId(), sharedId);

  ASSERT_THAT(*roots.at(1), SizeIs(1));
  EXPECT_EQ(roots.at(1)->childAt(0)->objectId(), sharedId);
  EXPECT_EQ(roots.at(1)->childAt(0), roots.at(0));

  ASSERT_THAT(*roots.at(2), SizeIs(1));
  EXPECT_EQ(roots.at(2)->childAt(0)->objectId(), sharedId);
  EXPECT_EQ(roots.at(1)->childAt(0), roots.at(2)->childAt(0));
}

// ── CpTable property ──────────────────────────────────────────────────────────

TEST_F(JsonSerializerTest, WriteSerializesCpTableProperty)
{
  auto item = std::make_shared<EpsilonItem>();
  item->setTable({{0, 0}, {1, 10}, {2, 25}});
  std::ostringstream out;
  serializer.write(out, {item});
  auto const& s = out.str();
  EXPECT_NE(s.find("std::map<int,int>"), std::string::npos);
  EXPECT_NE(s.find("10"), std::string::npos);
  EXPECT_NE(s.find("25"), std::string::npos);
}

TEST_F(JsonSerializerTest, RoundTripPreservesCpTableProperty)
{
  gurps_system::CpTable const original{{0, 0}, {1, 10}, {2, 25}, {3, 45}};
  auto item = std::make_shared<EpsilonItem>();
  item->setTable(original);

  std::ostringstream out;
  serializer.write(out, {item});

  infrastructure::ObjectRegistry reg2;
  auto items = readAll(serializer, out.str(), factory, reg2);
  ASSERT_EQ(items.size(), 1u);
  auto* restored = qobject_cast<EpsilonItem*>(items[0].get());
  ASSERT_NE(restored, nullptr);
  EXPECT_EQ(restored->table(), original);
}

// ── QList<int> property ───────────────────────────────────────────────────────

TEST_F(JsonSerializerTest, WriteSerializesQListIntProperty)
{
  auto item = std::make_shared<ZetaItem>();
  item->setValues({1, 2, 3, 5, 8});
  std::ostringstream out;
  serializer.write(out, {item});
  auto const& s = out.str();
  EXPECT_NE(s.find("QList<int>"), std::string::npos);
  EXPECT_NE(s.find("5"), std::string::npos);
  EXPECT_NE(s.find("8"), std::string::npos);
}

TEST_F(JsonSerializerTest, RoundTripPreservesQListIntProperty)
{
  QList<int> const original{10, 20, 30, 40};
  auto item = std::make_shared<ZetaItem>();
  item->setValues(original);

  std::ostringstream out;
  serializer.write(out, {item});

  infrastructure::ObjectRegistry reg2;
  auto items = readAll(serializer, out.str(), factory, reg2);
  ASSERT_EQ(items.size(), 1u);
  auto* restored = qobject_cast<ZetaItem*>(items[0].get());
  ASSERT_NE(restored, nullptr);
  EXPECT_EQ(restored->values(), original);
}
