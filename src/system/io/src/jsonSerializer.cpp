#include <jsonSerializer.hpp>

#include <cpTable.hpp>
#include <itemResolver.hpp>
#include <objectFactory.hpp>
#include <objectRegistry.hpp>
#include <treeItem.hpp>

#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_serialize.hpp>

#include <QByteArray>
#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaProperty>

#include <functional>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace gurps_system {

namespace {

// ── Constants ────────────────────────────────────────────────────────────────

static const QLatin1String kTypeId{"typeId"};
static const QLatin1String kObjectId{"objectId"};
static const QLatin1String kChildren{"children"};
static const QLatin1String kRoot{"root"};
static const QLatin1String kProperties{"properties"};
static const QLatin1String kPropType{"type"};
static const QLatin1String kPropValue{"value"};

// ── Property codecs ────────────────────────────────────────────────────────────────

struct PropCodec
{
  std::function<QJsonValue(QVariant const&)> write;
  std::function<QVariant(QJsonValue const&)> read;
};

// Lookup table mapping Qt type names to their JSON codec.
// Extend by adding entries here; no other code needs to change.
auto propCodecs() -> std::unordered_map<std::string, PropCodec> const&
{
  static const std::unordered_map<std::string, PropCodec> kTable{
      {"int",
       {[](QVariant const& v) -> QJsonValue { return v.toInt(); },
        [](QJsonValue const& j) -> QVariant { return j.toInt(); }}},
      {"QString",
       {[](QVariant const& v) -> QJsonValue { return v.toString(); },
        [](QJsonValue const& j) -> QVariant { return j.toString(); }}},
      {"QStringList",
       {[](QVariant const& v) -> QJsonValue {
          QJsonArray a;
          for (auto const& s : v.toStringList()) {
            a.append(s);
          }
          return a;
        },
        [](QJsonValue const& j) -> QVariant {
          QStringList sl;
          for (auto const& sv : j.toArray()) {
            sl.append(sv.toString());
          }
          return sl;
        }}},
      {"std::map<int,int>",
       {[](QVariant const& v) -> QJsonValue {
          auto const table = v.value<gurps_system::CpTable>();
          QJsonObject obj;
          for (auto const& [key, val] : table) {
            obj[QString::number(key)] = val;
          }
          return obj;
        },
        [](QJsonValue const& j) -> QVariant {
          gurps_system::CpTable table;
          auto const obj = j.toObject();
          for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
            table.emplace(it.key().toInt(), it.value().toInt());
          }
          return QVariant::fromValue(table);
        }}},
  };
  return kTable;
}

// ── UUID helpers ─────────────────────────────────────────────────────────────

auto uuidToString(boost::uuids::uuid const& id) -> QString
{
  return QString::fromStdString(to_string(id));
}

auto stringToUuid(QString const& s) -> boost::uuids::uuid
{
  return boost::uuids::string_generator{}(s.toStdString());
}

// ── DFS collect ───────────────────────────────────────────────────────────────

auto addItemToArray(QJsonArray& arr, std::shared_ptr<infrastructure::TreeItem> const& item,
                    std::set<boost::uuids::uuid>& seen,
                    std::set<boost::uuids::uuid> const& topLevel) -> void
{
  if (!item || !seen.insert(item->objectId()).second) {
    return;
  }
  QJsonObject obj;
  obj[kTypeId] = uuidToString(item->typeId());
  obj[kObjectId] = uuidToString(item->objectId());
  if (auto const it = topLevel.find(item->objectId()); it != topLevel.end()) {
    obj[kRoot] = true;
  }

  // Serialize Qt properties
  {
    auto const* mo = item->metaObject();
    auto const& codecs = propCodecs();
    QJsonObject propsObj;
    for (int i = infrastructure::TreeItem::staticMetaObject.propertyCount();
         i < mo->propertyCount(); ++i) {
      auto const prop = mo->property(i);
      if (!prop.isReadable() || !prop.isWritable()) {
        continue;
      }
      auto const codecIt = codecs.find(prop.typeName());
      if (codecIt == codecs.end()) {
        throw std::invalid_argument{
            std::string{"JsonSerializer::write: unsupported property type '"} + prop.typeName() +
            "'"};
      }
      QJsonObject taggedVal;
      taggedVal[kPropType] = QString::fromLatin1(prop.typeName());
      taggedVal[kPropValue] = codecIt->second.write(prop.read(item.get()));
      propsObj[QString::fromLatin1(prop.name())] = taggedVal;
    }
    if (!propsObj.isEmpty()) {
      obj[kProperties] = propsObj;
    }
  }

  if (item->size() > 0) {
    QJsonArray children;
    for (int i = 0; i < item->size(); ++i) {
      auto child = item->childAt(i);
      children.push_back(uuidToString(child->objectId()));
      addItemToArray(arr, child, seen, topLevel);
    }
    obj[kChildren] = std::move(children);
  }
  arr.push_back(std::move(obj));
}

auto objectToItem(QJsonObject const& obj, infrastructure::ObjectFactory& factory,
                  infrastructure::ObjectRegistry& registry)
    -> std::pair<bool, std::shared_ptr<infrastructure::TreeItem>>
{
  // First, create the item itself and register it so that resolvers can find it when wiring
  // children.
  auto const typeUuid = stringToUuid(obj[kTypeId].toString());
  auto const objUuid = stringToUuid(obj[kObjectId].toString());
  auto item = factory.create(typeUuid, objUuid);
  if (!item) {
    return {false, nullptr};
  }
  registry.registerObject(item);

  // Restore Qt properties
  {
    auto const& propsObj = obj[kProperties].toObject();
    auto const& codecs = propCodecs();
    for (auto it = propsObj.constBegin(); it != propsObj.constEnd(); ++it) {
      if (!it.value().isObject()) {
        continue;
      }
      auto const taggedVal = it.value().toObject();
      auto const typeName = taggedVal[kPropType].toString().toStdString();
      auto const codecIt = codecs.find(typeName);
      if (codecIt == codecs.end()) {
        throw std::invalid_argument{"JsonSerializer::read: unsupported property type '" + typeName +
                                    "'"};
      }
      auto const idx = item->metaObject()->indexOfProperty(it.key().toLocal8Bit().constData());
      if (idx < 0) {
        continue;
      }
      auto const prop = item->metaObject()->property(idx);
      if (!prop.isWritable()) {
        continue;
      }
      prop.write(item.get(), codecIt->second.read(taggedVal[kPropValue]));
    }
  }

  // Second, create ItemResolvers for the children and insert them as placeholders.  They will swap
  // themselves for the real child objects once they appear in the registry.
  for (auto const& childVal : obj[kChildren].toArray()) {
    auto const childId = stringToUuid(childVal.toString());
    item->insertChild(item->size(),
                      std::make_shared<infrastructure::ItemResolver>(childId, registry));
  }

  // Finally, return the item along with the "is root" marker (defaulting to false if missing) so
  // that
  auto isRoot = obj.contains(kRoot) ? obj[kRoot].toBool(false) : false;

  return {isRoot, item};
}

} // anonymous namespace

// ── Impl ──────────────────────────────────────────────────────────────────────

struct JsonSerializer::Impl
{
};

// ── Construction ──────────────────────────────────────────────────────────────

JsonSerializer::JsonSerializer() : Serializer{}, _p{std::make_unique<Impl>()} {}

JsonSerializer::~JsonSerializer() = default;

// ── Identity ──────────────────────────────────────────────────────────────────

auto JsonSerializer::classId() -> boost::uuids::uuid
{
  static auto const id = boost::uuids::string_generator{}("f0e9d8c7-b6a5-4f4e-3d2c-1b0a9f8e7d6c");
  return id;
}

auto JsonSerializer::id() const -> boost::uuids::uuid { return classId(); }

// ── Capabilities ──────────────────────────────────────────────────────────────

auto JsonSerializer::filters() const -> QStringList { return {"GURPS JSON (*.json)", "*.json"}; }

auto JsonSerializer::provides(QString const& filter) const -> bool
{
  return filters().contains(filter);
}

// ── IO ────────────────────────────────────────────────────────────────────────

auto JsonSerializer::read(Serializer::FileHeader const& /*header*/, uint16_t /*version*/,
                          std::istream& stream, infrastructure::ObjectFactory& factory,
                          infrastructure::ObjectRegistry& registry) const
    -> std::vector<std::shared_ptr<infrastructure::TreeItem>>
{
  std::string content(std::istreambuf_iterator<char>{stream}, {});
  auto const doc = QJsonDocument::fromJson(QByteArray::fromStdString(content));
  if (!doc.isArray()) {
    return {};
  }

  // Phase 1: create all items and collect child-UUID lists and root markers
  std::vector<std::shared_ptr<infrastructure::TreeItem>> result;
  std::vector<std::shared_ptr<infrastructure::TreeItem>> store;

  for (auto const& val : doc.array()) {
    if (!val.isObject()) {
      continue;
    }
    auto [isRoot, item] = objectToItem(val.toObject(), factory, registry);
    if (!item) {
      continue;
    }

    if (isRoot) {
      result.push_back(item);
    } else {
      store.push_back(item);
    }
  }

  QCoreApplication::processEvents(); // Let resolvers swap themselves for real items while we're
                                     // still alive
  return result;
}

auto JsonSerializer::write(
    std::ostream& stream,
    std::vector<std::shared_ptr<infrastructure::TreeItem>> const& objects) const -> void
{
  // Write the binary file header
  auto const header = Serializer::buildHeader(classId(), 1);
  stream.write(reinterpret_cast<char const*>(header.data()), Serializer::kHeaderSize);

  // Non-explicit-root children are still assigned to only one parent to avoid
  // duplicating tree structure.  Explicit-root children may appear under many.
  std::set<boost::uuids::uuid> assignedChildren;
  std::set<boost::uuids::uuid> explicitRootChildren;
  std::ranges::transform(objects, std::inserter(explicitRootChildren, explicitRootChildren.end()),
                         [](auto const& item) { return item->objectId(); });

  QJsonArray arr;
  for (auto const& item : objects) {
    // Add children recursively to array
    addItemToArray(arr, item, assignedChildren, explicitRootChildren);
  }
  auto const bytes = QJsonDocument{arr}.toJson(QJsonDocument::Indented);
  stream.write(bytes.constData(), bytes.size());
}

} // namespace gurps_system
