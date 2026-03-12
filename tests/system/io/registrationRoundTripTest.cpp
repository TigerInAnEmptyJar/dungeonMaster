#include <jsonSerializer.hpp>
#include <xmlSerializer.hpp>

#include <objectFactory.hpp>
#include <objectRegistry.hpp>
#include <registration.hpp>
#include <serializer.hpp>

#include <gtest/gtest.h>

#include <memory>
#include <sstream>
#include <vector>

// ── QCoreApplication is provided by ioTest.cpp (same test binary) ─────────────

using namespace gurps_system;

namespace {

auto parseHeader(std::string const& data) -> std::pair<Serializer::FileHeader, uint16_t>
{
  Serializer::FileHeader header{};
  std::istringstream in{data};
  in.read(reinterpret_cast<char*>(header.data()), Serializer::kHeaderSize);
  auto const version =
      static_cast<uint16_t>((static_cast<uint16_t>(std::to_integer<uint8_t>(header[21])) << 8) |
                            static_cast<uint16_t>(std::to_integer<uint8_t>(header[22])));
  return {header, version};
}

} // namespace

// ── Fixture ───────────────────────────────────────────────────────────────────

class RegistrationRoundTripTest : public ::testing::Test
{
protected:
  infrastructure::ObjectFactory factory;

  auto SetUp() -> void override { registerSystemObjects(factory); }

  // Creates one default-constructed instance of every registered type.
  auto allInstances() const -> std::vector<std::shared_ptr<infrastructure::TreeItem>>
  {
    std::vector<std::shared_ptr<infrastructure::TreeItem>> items;
    for (auto const& typeId : factory.installedTypes()) {
      items.push_back(factory.create(typeId));
    }
    return items;
  }
};

// ── JSON ──────────────────────────────────────────────────────────────────────

TEST_F(RegistrationRoundTripTest, JsonRoundTripAllRegisteredTypesDoesNotThrow)
{
  JsonSerializer ser;
  auto const items = allInstances();

  std::ostringstream out;
  ASSERT_NO_THROW(ser.write(out, items));

  auto const [header, version] = parseHeader(out.str());
  std::istringstream in{out.str()};
  in.seekg(Serializer::kHeaderSize);

  infrastructure::ObjectRegistry reg;
  EXPECT_NO_THROW(ser.read(header, version, in, factory, reg));
}

// ── XML ───────────────────────────────────────────────────────────────────────

TEST_F(RegistrationRoundTripTest, XmlRoundTripAllRegisteredTypesDoesNotThrow)
{
  XmlSerializer ser;
  auto const items = allInstances();

  std::ostringstream out;
  ASSERT_NO_THROW(ser.write(out, items));

  auto const [header, version] = parseHeader(out.str());
  std::istringstream in{out.str()};
  in.seekg(Serializer::kHeaderSize);

  infrastructure::ObjectRegistry reg;
  EXPECT_NO_THROW(ser.read(header, version, in, factory, reg));
}
