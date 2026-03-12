#include <xmlSerializer.hpp>

#include <cpTable.hpp>
#include <itemResolver.hpp>
#include <objectFactory.hpp>
#include <objectRegistry.hpp>
#include <treeItem.hpp>

#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <QBuffer>
#include <QByteArray>
#include <QCoreApplication>
#include <QMetaProperty>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <functional>
#include <ranges>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace gurps_system {

namespace {

// ── Property codecs ───────────────────────────────────────────────────────────

struct XmlPropCodec
{
  // Called while inside an open <property> element; writes the value content.
  std::function<void(QXmlStreamWriter&, QVariant const&)> write;

  // Called when xml is AT the <property> StartElement.
  // Reads the element content and leaves xml AT the </property> EndElement.
  std::function<QVariant(QXmlStreamReader&)> read;
};

// Lookup table mapping Qt type names to their XML codec.
// Extend by adding entries here; no other code needs to change.
auto propCodecs() -> std::unordered_map<std::string, XmlPropCodec> const&
{
  static const std::unordered_map<std::string, XmlPropCodec> kTable{
      {"int",
       {[](QXmlStreamWriter& xml, QVariant const& v) {
          xml.writeCharacters(QString::number(v.toInt()));
        },
        [](QXmlStreamReader& xml) -> QVariant {
          return xml.readElementText().toInt(); // leaves AT </property>
        }}},
      {"QString",
       {[](QXmlStreamWriter& xml, QVariant const& v) { xml.writeCharacters(v.toString()); },
        [](QXmlStreamReader& xml) -> QVariant {
          return xml.readElementText(); // leaves AT </property>
        }}},
      {"QStringList",
       {[](QXmlStreamWriter& xml, QVariant const& v) {
          for (auto const& s : v.toStringList()) {
            xml.writeTextElement(QStringLiteral("entry"), s);
          }
        },
        [](QXmlStreamReader& xml) -> QVariant {
          QStringList sl;
          while (xml.readNextStartElement()) { // inside <property>
            if (xml.name() == QLatin1String("entry")) {
              sl.append(xml.readElementText()); // leaves AT </entry>
            } else {
              xml.skipCurrentElement();
            }
          }
          return sl; // readNextStartElement hit </property>; reader AT </property>
        }}},
      {"std::map<int,int>",
       {[](QXmlStreamWriter& xml, QVariant const& v) {
          auto const table = v.value<gurps_system::CpTable>();
          for (auto const& [key, val] : table) {
            xml.writeStartElement(QStringLiteral("entry"));
            xml.writeAttribute(QStringLiteral("key"), QString::number(key));
            xml.writeAttribute(QStringLiteral("value"), QString::number(val));
            xml.writeEndElement();
          }
        },
        [](QXmlStreamReader& xml) -> QVariant {
          gurps_system::CpTable table;
          while (xml.readNextStartElement()) { // inside <property>
            if (xml.name() == QLatin1String("entry")) {
              auto const key = xml.attributes().value(QStringLiteral("key")).toInt();
              auto const val = xml.attributes().value(QStringLiteral("value")).toInt();
              xml.skipCurrentElement(); // consume <entry/>
              table.emplace(key, val);
            } else {
              xml.skipCurrentElement();
            }
          }
          return QVariant::fromValue(table); // reader AT </property>
        }}},
  };
  return kTable;
}

// ── UUID helpers ──────────────────────────────────────────────────────────────

auto uuidToString(boost::uuids::uuid const& id) -> QString
{
  return QString::fromStdString(to_string(id));
}

auto stringToUuid(QString const& s) -> boost::uuids::uuid
{
  return boost::uuids::string_generator{}(s.toStdString());
}

// ── Write (post-order DFS, flat <items> list) ─────────────────────────────────

auto addItemToXml(QXmlStreamWriter& xml, std::shared_ptr<infrastructure::TreeItem> const& item,
                  std::set<boost::uuids::uuid>& seen, std::set<boost::uuids::uuid> const& topLevel)
    -> void
{
  if (!item || !seen.insert(item->objectId()).second) {
    return;
  }

  // Recurse into children first so they appear before the parent in the flat list.
  for (int i = 0; i < item->size(); ++i) {
    addItemToXml(xml, item->childAt(i), seen, topLevel);
  }

  xml.writeStartElement(QStringLiteral("item"));
  xml.writeAttribute(QStringLiteral("typeId"), uuidToString(item->typeId()));
  xml.writeAttribute(QStringLiteral("objectId"), uuidToString(item->objectId()));
  if (topLevel.count(item->objectId())) {
    xml.writeAttribute(QStringLiteral("root"), QStringLiteral("true"));
  }

  // Serialize Qt properties
  auto const* mo = item->metaObject();
  auto const& codecs = propCodecs();
  bool propsOpen = false;
  for (int i = infrastructure::TreeItem::staticMetaObject.propertyCount(); i < mo->propertyCount();
       ++i) {
    auto const prop = mo->property(i);
    if (!prop.isReadable() || !prop.isWritable()) {
      continue;
    }
    auto const codecIt = codecs.find(prop.typeName());
    if (codecIt == codecs.end()) {
      throw std::invalid_argument{std::string{"XmlSerializer::write: unsupported property type '"} +
                                  prop.typeName() + "'"};
    }
    if (!propsOpen) {
      xml.writeStartElement(QStringLiteral("properties"));
      propsOpen = true;
    }
    xml.writeStartElement(QStringLiteral("property"));
    xml.writeAttribute(QStringLiteral("name"), QString::fromLatin1(prop.name()));
    xml.writeAttribute(QStringLiteral("type"), QString::fromLatin1(prop.typeName()));
    codecIt->second.write(xml, prop.read(item.get()));
    xml.writeEndElement(); // </property>
  }
  if (propsOpen) {
    xml.writeEndElement(); // </properties>
  }

  // Serialize child UUID references
  if (item->size() > 0) {
    xml.writeStartElement(QStringLiteral("children"));
    for (int i = 0; i < item->size(); ++i) {
      xml.writeTextElement(QStringLiteral("child"), uuidToString(item->childAt(i)->objectId()));
    }
    xml.writeEndElement(); // </children>
  }

  xml.writeEndElement(); // </item>
}

// ── Read ──────────────────────────────────────────────────────────────────────

// xml is AT the <item> StartElement on entry.
// On exit, xml is AT the </item> EndElement (or past it if factory returned null).
auto objectToItem(QXmlStreamReader& xml, infrastructure::ObjectFactory& factory,
                  infrastructure::ObjectRegistry& registry)
    -> std::pair<bool, std::shared_ptr<infrastructure::TreeItem>>
{
  auto const typeUuid = stringToUuid(xml.attributes().value(QStringLiteral("typeId")).toString());
  auto const objUuid = stringToUuid(xml.attributes().value(QStringLiteral("objectId")).toString());
  auto const isRoot = xml.attributes().value(QStringLiteral("root")) == QLatin1String("true");

  auto item = factory.create(typeUuid, objUuid);
  if (!item) {
    xml.skipCurrentElement(); // skip <item>…</item>; leaves AT </item>
    return {false, nullptr};
  }
  registry.registerObject(item);

  auto const& codecs = propCodecs();
  std::vector<boost::uuids::uuid> childIds;

  while (xml.readNextStartElement()) { // iterate direct children of <item>
    if (xml.name() == QLatin1String("properties")) {
      while (xml.readNextStartElement()) { // iterate <property> elements
        if (xml.name() == QLatin1String("property")) {
          // Copy attribute strings before advancing the reader
          auto const propName = xml.attributes().value(QStringLiteral("name")).toLatin1();
          auto const typeName =
              xml.attributes().value(QStringLiteral("type")).toString().toStdString();
          auto const codecIt = codecs.find(typeName);
          if (codecIt == codecs.end()) {
            throw std::invalid_argument{"XmlSerializer::read: unsupported property type '" +
                                        typeName + "'"};
          }
          auto const value = codecIt->second.read(xml); // leaves AT </property>
          auto const idx = item->metaObject()->indexOfProperty(propName.constData());
          if (idx >= 0) {
            auto const prop = item->metaObject()->property(idx);
            if (prop.isWritable()) {
              prop.write(item.get(), value);
            }
          }
        } else {
          xml.skipCurrentElement();
        }
      }
      // reader AT </properties>
    } else if (xml.name() == QLatin1String("children")) {
      while (xml.readNextStartElement()) { // iterate <child> elements
        if (xml.name() == QLatin1String("child")) {
          childIds.push_back(stringToUuid(xml.readElementText())); // leaves AT </child>
        } else {
          xml.skipCurrentElement();
        }
      }
      // reader AT </children>
    } else {
      xml.skipCurrentElement();
    }
  }
  // reader AT </item>

  for (auto const& childId : childIds) {
    item->insertChild(item->size(),
                      std::make_shared<infrastructure::ItemResolver>(childId, registry));
  }

  return {isRoot, item};
}

} // anonymous namespace

// ── Impl ──────────────────────────────────────────────────────────────────────

struct XmlSerializer::Impl
{
};

// ── Construction ──────────────────────────────────────────────────────────────

XmlSerializer::XmlSerializer() : Serializer{}, _p{std::make_unique<Impl>()} {}

XmlSerializer::~XmlSerializer() = default;

// ── Identity ──────────────────────────────────────────────────────────────────

auto XmlSerializer::classId() -> boost::uuids::uuid
{
  static auto const id = boost::uuids::string_generator{}("a0b1c2d3-e4f5-4a6b-8c7d-e8f9a0b1c2d3");
  return id;
}

auto XmlSerializer::id() const -> boost::uuids::uuid { return classId(); }

// ── Capabilities ──────────────────────────────────────────────────────────────

auto XmlSerializer::filters() const -> QStringList { return {"GURPS XML (*.xml)", "*.xml"}; }

auto XmlSerializer::provides(QString const& filter) const -> bool
{
  return filters().contains(filter);
}

// ── IO ────────────────────────────────────────────────────────────────────────

auto XmlSerializer::read(Serializer::FileHeader const& /*header*/, uint16_t /*version*/,
                         std::istream& stream, infrastructure::ObjectFactory& factory,
                         infrastructure::ObjectRegistry& registry) const
    -> std::vector<std::shared_ptr<infrastructure::TreeItem>>
{
  std::string content(std::istreambuf_iterator<char>{stream}, {});
  QXmlStreamReader xml{QByteArray::fromStdString(content)};

  // Expect the root element to be <items>
  if (!xml.readNextStartElement() || xml.name() != QLatin1String("items")) {
    return {};
  }

  std::vector<std::shared_ptr<infrastructure::TreeItem>> result;
  std::vector<std::shared_ptr<infrastructure::TreeItem>> store;

  while (xml.readNextStartElement()) { // iterate <item> siblings inside <items>
    if (xml.name() != QLatin1String("item")) {
      xml.skipCurrentElement();
      continue;
    }
    auto [isRoot, item] = objectToItem(xml, factory, registry);
    if (!item) {
      continue;
    }

    if (isRoot) {
      result.push_back(item);
    } else {
      store.push_back(item);
    }
  }

  QCoreApplication::processEvents(); // let resolvers swap in real child objects
  return result;
}

auto XmlSerializer::write(
    std::ostream& stream,
    std::vector<std::shared_ptr<infrastructure::TreeItem>> const& objects) const -> void
{
  // Write the binary file header
  auto const header = Serializer::buildHeader(classId(), 1);
  stream.write(reinterpret_cast<char const*>(header.data()), Serializer::kHeaderSize);

  QByteArray ba;
  QBuffer buf{&ba};
  buf.open(QIODevice::WriteOnly);
  QXmlStreamWriter xml{&buf};
  xml.setAutoFormatting(true);
  xml.writeStartDocument();
  xml.writeStartElement(QStringLiteral("items"));

  std::set<boost::uuids::uuid> seen;
  std::set<boost::uuids::uuid> topLevel;
  std::ranges::transform(objects, std::inserter(topLevel, topLevel.end()),
                         [](auto const& item) { return item->objectId(); });

  for (auto const& item : objects) {
    addItemToXml(xml, item, seen, topLevel);
  }

  xml.writeEndElement(); // </items>
  xml.writeEndDocument();
  buf.close();

  stream.write(ba.constData(), ba.size());
}

} // namespace gurps_system
