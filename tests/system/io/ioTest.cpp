#include <io.hpp>
#include <serializer.hpp>

#include <objectFactory.hpp>
#include <objectRegistry.hpp>
#include <treeItem.hpp>

#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid.hpp>

#include <QCoreApplication>

#include <gtest/gtest.h>

#include <memory>
#include <sstream>

// ── QCoreApplication ──────────────────────────────────────────────────────────

static int argc_storage = 0;
static char** argv_storage = nullptr;

static QCoreApplication app{argc_storage, argv_storage};

// ── Test double ───────────────────────────────────────────────────────────────
// A minimal concrete Serializer used to exercise Io behaviour.

namespace {

static const boost::uuids::uuid kJsonId =
    boost::uuids::string_generator{}("11111111-2222-4333-8444-555555555555");

static const boost::uuids::uuid kXmlId =
    boost::uuids::string_generator{}("aaaaaaaa-bbbb-4ccc-8ddd-eeeeeeeeeeee");

class StubSerializer : public gurps_system::Serializer
{
public:
  explicit StubSerializer(boost::uuids::uuid id, QStringList filters, char detectChar = '{')
      : _id{id}, _filters{std::move(filters)}, _detectChar{detectChar}
  {
  }

  auto id() const -> boost::uuids::uuid override { return _id; }
  auto filters() const -> QStringList override { return _filters; }
  auto provides(QString const& filter) const -> bool override { return _filters.contains(filter); }
  auto canRead(std::string const& firstLine) const -> bool override
  {
    return !firstLine.empty() && firstLine.front() == _detectChar;
  }
  auto read(std::string const&, std::istream&, infrastructure::ObjectFactory&,
            infrastructure::ObjectRegistry&) const
      -> std::shared_ptr<infrastructure::TreeItem> override
  {
    readCalled = true;
    return nullptr;
  }
  auto write(std::ostream&, infrastructure::TreeItem const&) const -> void override
  {
    writeCalled = true;
  }

  mutable bool readCalled{false};
  mutable bool writeCalled{false};

private:
  boost::uuids::uuid _id;
  QStringList _filters;
  char _detectChar;
};

auto makeJson() -> std::shared_ptr<StubSerializer>
{
  return std::make_shared<StubSerializer>(kJsonId, QStringList{"GURPS JSON (*.json)", "*.json"});
}

auto makeXml() -> std::shared_ptr<StubSerializer>
{
  return std::make_shared<StubSerializer>(kXmlId, QStringList{"GURPS XML (*.xml)", "*.xml"}, '<');
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

TEST_F(IoTest, InstallReturnsTrueForNewSerializer) { EXPECT_TRUE(io.install(makeJson())); }

TEST_F(IoTest, InstallReturnsFalseForDuplicateId)
{
  io.install(makeJson());
  EXPECT_FALSE(io.install(makeJson()));
}

TEST_F(IoTest, InstallReturnsFalseForNullptr) { EXPECT_FALSE(io.install(nullptr)); }

// ── uninstall ─────────────────────────────────────────────────────────────────

TEST_F(IoTest, UninstallReturnsTrueForInstalledSerializer)
{
  io.install(makeJson());
  EXPECT_TRUE(io.uninstall(kJsonId));
}

TEST_F(IoTest, UninstallReturnsFalseForUnknownId) { EXPECT_FALSE(io.uninstall(kJsonId)); }

TEST_F(IoTest, UninstalledSerializerIsNoLongerFound)
{
  io.install(makeJson());
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
  io.install(makeJson());
  auto ids = io.installedSerializers();
  EXPECT_EQ(ids.size(), 1u);
  EXPECT_EQ(ids[0], kJsonId);
}

TEST_F(IoTest, InstalledSerializersReflectsMultipleInstalls)
{
  io.install(makeJson());
  io.install(makeXml());
  EXPECT_EQ(io.installedSerializers().size(), 2u);
}

// ── serializer(uuid) ─────────────────────────────────────────────────────────

TEST_F(IoTest, SerializerByIdReturnsNullForUnknownId)
{
  EXPECT_EQ(io.serializer(kJsonId), nullptr);
}

TEST_F(IoTest, SerializerByIdReturnsCorrectSerializer)
{
  auto s = makeJson();
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
  io.install(s);
  EXPECT_EQ(io.serializer("*.json"), s.get());
}

TEST_F(IoTest, SerializerByFilterDoesNotMatchOtherSerializer)
{
  io.install(makeJson());
  EXPECT_EQ(io.serializer("*.xml"), nullptr);
}

// ── allFilters ────────────────────────────────────────────────────────────────

TEST_F(IoTest, AllFiltersIsEmptyInitially) { EXPECT_TRUE(io.allFilters().isEmpty()); }

TEST_F(IoTest, AllFiltersContainsFiltersFromInstalledSerializers)
{
  io.install(makeJson());
  io.install(makeXml());
  auto f = io.allFilters();
  EXPECT_TRUE(f.contains("GURPS JSON (*.json)"));
  EXPECT_TRUE(f.contains("GURPS XML (*.xml)"));
}

TEST_F(IoTest, AllFiltersExcludesUninstalledSerializer)
{
  io.install(makeJson());
  io.uninstall(kJsonId);
  EXPECT_TRUE(io.allFilters().isEmpty());
}

// ── Serializer interface ──────────────────────────────────────────────────────

TEST(SerializerTest, CanReadDetectsJsonFormat)
{
  StubSerializer s{kJsonId, {}};
  EXPECT_TRUE(s.canRead("{\"type\":\"attribute\"}"));
  EXPECT_FALSE(s.canRead("<xml>"));
}

TEST(SerializerTest, ProvidesMatchesExactFilter)
{
  StubSerializer s{kJsonId, QStringList{"GURPS JSON (*.json)"}};
  EXPECT_TRUE(s.provides("GURPS JSON (*.json)"));
  EXPECT_FALSE(s.provides("GURPS XML (*.xml)"));
}

// ── Io::read forwarding ───────────────────────────────────────────────────────

TEST_F(IoTest, ReadReturnsNullptrForEmptyStream)
{
  io.install(makeJson());
  std::istringstream stream;
  EXPECT_EQ(io.read(stream), nullptr);
}

TEST_F(IoTest, ReadReturnsNullptrWhenNoSerializerMatches)
{
  io.install(makeJson());
  std::istringstream stream{"<not-json>\nrest"};
  EXPECT_EQ(io.read(stream), nullptr);
}

TEST_F(IoTest, ReadDelegatesToMatchingSerializer)
{
  auto json = makeJson();
  io.install(json);
  std::istringstream stream{"{\"type\":\"attribute\"}\nrest"};
  io.read(stream);
  EXPECT_TRUE(json->readCalled);
}

TEST_F(IoTest, ReadSelectsCorrectSerializerAmongMultiple)
{
  auto json = makeJson();
  auto xml = makeXml();
  io.install(json);
  io.install(xml);
  std::istringstream stream{"<root/>\nrest"};
  io.read(stream);
  EXPECT_FALSE(json->readCalled);
  EXPECT_TRUE(xml->readCalled);
}

// ── Io::write forwarding ───────────────────────────────────────────────────────

TEST_F(IoTest, WriteByFilterThrowsForUnknownFilter)
{
  auto item = std::make_shared<infrastructure::TreeItem>();
  std::ostringstream stream;
  EXPECT_THROW(io.write(stream, *item, "*.json"), std::invalid_argument);
}

TEST_F(IoTest, WriteByFilterDelegatesToMatchingSerializer)
{
  auto json = makeJson();
  io.install(json);
  auto item = std::make_shared<infrastructure::TreeItem>();
  std::ostringstream stream;
  io.write(stream, *item, "*.json");
  EXPECT_TRUE(json->writeCalled);
}

TEST_F(IoTest, WriteByUuidThrowsForUnknownId)
{
  auto item = std::make_shared<infrastructure::TreeItem>();
  std::ostringstream stream;
  EXPECT_THROW(io.write(stream, *item, kJsonId), std::invalid_argument);
}

TEST_F(IoTest, WriteByUuidDelegatesToMatchingSerializer)
{
  auto json = makeJson();
  io.install(json);
  auto item = std::make_shared<infrastructure::TreeItem>();
  std::ostringstream stream;
  io.write(stream, *item, kJsonId);
  EXPECT_TRUE(json->writeCalled);
}