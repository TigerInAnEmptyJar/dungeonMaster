#include <io.hpp>
#include <serializer.hpp>

#include <objectFactory.hpp>
#include <objectRegistry.hpp>
#include <treeItem.hpp>

#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid.hpp>

#include <QCoreApplication>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <sstream>
#include <vector>

using testing::_;
using testing::ElementsAre;
using testing::IsEmpty;
using testing::Return;
using testing::SizeIs;
using testing::StrictMock;
using testing::UnorderedElementsAre;

// ── QCoreApplication ──────────────────────────────────────────────────────────

static int argc_storage = 0;
static char** argv_storage = nullptr;

static QCoreApplication app{argc_storage, argv_storage};

// ── Test double ───────────────────────────────────────────────────────────────

namespace {

static const boost::uuids::uuid kJsonId =
    boost::uuids::string_generator{}("11111111-2222-4333-8444-555555555555");

static const boost::uuids::uuid kXmlId =
    boost::uuids::string_generator{}("aaaaaaaa-bbbb-4ccc-8ddd-eeeeeeeeeeee");

class SerializerMock : public gurps_system::Serializer
{
public:
  MOCK_METHOD(boost::uuids::uuid, id, (), (const, override));
  MOCK_METHOD(QStringList, filters, (), (const, override));
  MOCK_METHOD(bool, provides, (QString const& filter), (const, override));
  MOCK_METHOD(std::vector<std::shared_ptr<infrastructure::TreeItem>>, read,
              (gurps_system::Serializer::FileHeader const& header, uint16_t version,
               std::istream& stream, infrastructure::ObjectFactory& factory,
               infrastructure::ObjectRegistry& registry),
              (const, override));
  MOCK_METHOD(void, write,
              (std::ostream & stream,
               std::vector<std::shared_ptr<infrastructure::TreeItem>> const& objects),
              (const, override));
};

auto makeJson() -> std::shared_ptr<SerializerMock>
{
  auto mock = std::make_shared<StrictMock<SerializerMock>>();
  ON_CALL(*mock, id()).WillByDefault(Return(kJsonId));
  ON_CALL(*mock, filters()).WillByDefault(Return(QStringList{"GURPS JSON (*.json)", "*.json"}));
  return mock;
}

auto makeXml() -> std::shared_ptr<SerializerMock>
{
  auto mock = std::make_shared<StrictMock<SerializerMock>>();
  ON_CALL(*mock, id()).WillByDefault(Return(kXmlId));
  ON_CALL(*mock, filters()).WillByDefault(Return(QStringList{"GURPS XML (*.xml)", "*.xml"}));
  return mock;
}

} // namespace

using namespace gurps_system;

// ── Fixture ───────────────────────────────────────────────────────────────────

class IoTest : public ::testing::Test
{
protected:
  infrastructure::ObjectFactory factory;
  infrastructure::ObjectRegistry registry;
  Io io{factory, registry};
};

// ── install ───────────────────────────────────────────────────────────────────

TEST_F(IoTest, InstallReturnsTrueForNewSerializer)
{
  auto serializer = makeJson();
  EXPECT_CALL(*serializer, id()).WillOnce(Return(kJsonId));
  io.install(serializer);
}

TEST_F(IoTest, InstallReturnsFalseForDuplicateId)
{
  auto serializer = makeJson();
  EXPECT_CALL(*serializer, id()).WillOnce(Return(kJsonId));
  io.install(serializer);
  auto serializer2 = makeJson();
  EXPECT_CALL(*serializer2, id()).WillOnce(Return(kJsonId));
  EXPECT_FALSE(io.install(serializer2));
}

TEST_F(IoTest, InstallReturnsFalseForNullptr) { EXPECT_FALSE(io.install(nullptr)); }

// ── uninstall ─────────────────────────────────────────────────────────────────

TEST_F(IoTest, UninstallReturnsTrueForInstalledSerializer)
{
  auto serializer = makeJson();
  EXPECT_CALL(*serializer, id()).WillOnce(Return(kJsonId));
  io.install(serializer);
  EXPECT_TRUE(io.uninstall(kJsonId));
}

TEST_F(IoTest, UninstallReturnsFalseForUnknownId) { EXPECT_FALSE(io.uninstall(kJsonId)); }

TEST_F(IoTest, UninstalledSerializerIsNoLongerFound)
{
  auto serializer = makeJson();
  EXPECT_CALL(*serializer, id()).WillOnce(Return(kJsonId));
  io.install(serializer);
  io.uninstall(kJsonId);
  EXPECT_EQ(io.serializer(kJsonId), nullptr);
}

// ── installedSerializers ──────────────────────────────────────────────────────

TEST_F(IoTest, InstalledSerializersIsEmptyInitially)
{
  EXPECT_TRUE(io.installedSerializers().empty());
}

TEST_F(IoTest, InstalledSerializersContainsInstalledId)
{
  auto serializer = makeJson();
  EXPECT_CALL(*serializer, id()).WillOnce(Return(kJsonId));
  io.install(serializer);
  auto ids = io.installedSerializers();
  EXPECT_THAT(ids, ElementsAre(kJsonId));
}

TEST_F(IoTest, InstalledSerializersReflectsMultipleInstalls)
{
  auto json = makeJson();
  auto xml = makeXml();
  EXPECT_CALL(*json, id()).WillOnce(Return(kJsonId));
  EXPECT_CALL(*xml, id()).WillOnce(Return(kXmlId));
  io.install(json);
  io.install(xml);
  EXPECT_THAT(io.installedSerializers(), UnorderedElementsAre(kJsonId, kXmlId));
}

// ── serializer(uuid) ─────────────────────────────────────────────────────────

TEST_F(IoTest, SerializerByIdReturnsNullForUnknownId)
{
  EXPECT_EQ(io.serializer(kJsonId), nullptr);
}

TEST_F(IoTest, SerializerByIdReturnsCorrectSerializer)
{
  auto s = makeJson();
  EXPECT_CALL(*s, id()).WillOnce(Return(kJsonId));
  io.install(s);
  EXPECT_EQ(io.serializer(kJsonId), s.get());
}

// ── serializer(filter) ────────────────────────────────────────────────────────

TEST_F(IoTest, SerializerByFilterReturnsNullForUnknownFilter)
{
  EXPECT_EQ(io.serializer("*.json"), nullptr);
}

TEST_F(IoTest, SerializerByFilterReturnsMatchingSerializer)
{
  auto s = makeJson();
  EXPECT_CALL(*s, id()).WillOnce(Return(kJsonId));
  EXPECT_CALL(*s, provides(_)).WillOnce(Return(true));
  io.install(s);
  EXPECT_EQ(io.serializer("*.json"), s.get());
}

TEST_F(IoTest, SerializerByFilterDoesNotMatchOtherSerializer)
{
  auto s = makeJson();
  EXPECT_CALL(*s, id()).WillOnce(Return(kJsonId));
  EXPECT_CALL(*s, provides(_)).WillOnce(Return(false));
  io.install(s);
  EXPECT_EQ(io.serializer("*.xml"), nullptr);
}

// ── allFilters ────────────────────────────────────────────────────────────────

TEST_F(IoTest, AllFiltersIsEmptyInitially) { EXPECT_TRUE(io.allFilters().isEmpty()); }

TEST_F(IoTest, AllFiltersContainsFiltersFromInstalledSerializers)
{
  auto json = makeJson();
  auto xml = makeXml();
  EXPECT_CALL(*json, id()).WillOnce(Return(kJsonId));
  EXPECT_CALL(*xml, id()).WillOnce(Return(kXmlId));
  EXPECT_CALL(*json, filters()).WillOnce(Return(QStringList{"GURPS JSON (*.json)", "*.json"}));
  EXPECT_CALL(*xml, filters()).WillOnce(Return(QStringList{"GURPS XML (*.xml)", "*.xml"}));
  io.install(json);
  io.install(xml);
  auto f = io.allFilters();
  EXPECT_THAT(f,
              UnorderedElementsAre("GURPS JSON (*.json)", "GURPS XML (*.xml)", "*.json", "*.xml"));
}

TEST_F(IoTest, AllFiltersExcludesUninstalledSerializer)
{
  auto json = makeJson();
  EXPECT_CALL(*json, id()).WillOnce(Return(kJsonId));
  io.install(json);
  io.uninstall(kJsonId);
  EXPECT_THAT(io.allFilters(), IsEmpty());
}

// ── Serializer interface ──────────────────────────────────────────────────────

TEST_F(IoTest, ReadReturnsEmptyForEmptyStream)
{
  auto json = makeJson();
  EXPECT_CALL(*json, id()).WillOnce(Return(kJsonId));
  io.install(json);
  std::istringstream stream;
  EXPECT_TRUE(io.read(stream).empty());
}

TEST_F(IoTest, ReadReturnsEmptyWhenNoSerializerMatches)
{
  auto json = makeJson();
  EXPECT_CALL(*json, id()).WillOnce(Return(kJsonId));
  io.install(json);
  // Build a header for kXmlId (not installed)
  auto const header = gurps_system::Serializer::buildHeader(kXmlId, 1);
  std::string const data{reinterpret_cast<char const*>(header.data()),
                         gurps_system::Serializer::kHeaderSize};
  std::istringstream stream{data};
  EXPECT_TRUE(io.read(stream).empty());
}

TEST_F(IoTest, ReadDelegatesToMatchingSerializer)
{
  auto json = makeJson();
  EXPECT_CALL(*json, id()).WillOnce(Return(kJsonId));
  io.install(json);
  auto const header = gurps_system::Serializer::buildHeader(kJsonId, 1);
  std::string const data{reinterpret_cast<char const*>(header.data()),
                         gurps_system::Serializer::kHeaderSize};
  std::istringstream stream{data};
  EXPECT_CALL(*json, read(_, _, _, _, _))
      .WillOnce(Return(std::vector<std::shared_ptr<infrastructure::TreeItem>>{}));
  io.read(stream);
}

TEST_F(IoTest, ReadSelectsCorrectSerializerAmongMultiple)
{
  auto json = makeJson();
  auto xml = makeXml();
  EXPECT_CALL(*json, id()).WillOnce(Return(kJsonId));
  EXPECT_CALL(*xml, id()).WillOnce(Return(kXmlId));
  io.install(json);
  io.install(xml);
  auto const header = gurps_system::Serializer::buildHeader(kXmlId, 1);
  std::string const data{reinterpret_cast<char const*>(header.data()),
                         gurps_system::Serializer::kHeaderSize};
  std::istringstream stream{data};
  EXPECT_CALL(*xml, read(_, _, _, _, _))
      .WillOnce(Return(std::vector<std::shared_ptr<infrastructure::TreeItem>>{}));
  io.read(stream);
}

// ── Io::write forwarding ──────────────────────────────────────────────────────

TEST_F(IoTest, WriteByFilterThrowsForUnknownFilter)
{
  std::vector<std::shared_ptr<infrastructure::TreeItem>> items;
  std::ostringstream stream;
  EXPECT_THROW(io.write(stream, items, "*.json"), std::invalid_argument);
}

TEST_F(IoTest, WriteByFilterDelegatesToMatchingSerializer)
{
  auto json = makeJson();
  EXPECT_CALL(*json, id()).WillOnce(Return(kJsonId));
  io.install(json);
  std::vector<std::shared_ptr<infrastructure::TreeItem>> items;
  std::ostringstream stream;
  EXPECT_CALL(*json, provides(QString("*.json"))).WillOnce(Return(true));
  EXPECT_CALL(*json, write(_, _));
  io.write(stream, items, "*.json");
}

TEST_F(IoTest, WriteByUuidThrowsForUnknownId)
{
  std::vector<std::shared_ptr<infrastructure::TreeItem>> items;
  std::ostringstream stream;
  EXPECT_THROW(io.write(stream, items, kJsonId), std::invalid_argument);
}

TEST_F(IoTest, WriteByUuidDelegatesToMatchingSerializer)
{
  auto json = makeJson();
  EXPECT_CALL(*json, id()).WillOnce(Return(kJsonId));
  io.install(json);
  std::vector<std::shared_ptr<infrastructure::TreeItem>> items;
  std::ostringstream stream;
  // EXPECT_CALL(*json, provides(_)).WillOnce(Return(true));
  EXPECT_CALL(*json, write(_, _)).WillOnce(testing::Return());
  io.write(stream, items, kJsonId);
}
