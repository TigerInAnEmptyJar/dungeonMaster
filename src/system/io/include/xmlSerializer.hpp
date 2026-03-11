#pragma once

#include <serializer.hpp>

#include <memory>
#include <vector>

namespace gurps_system {

/**
 * \brief XML serializer for TreeItem-derived objects.
 *
 * Writes a 23-byte binary header (magic \c "DMFMT" + UUID + version) followed
 * by a UTF-8 XML document.  The document has an \c <items> root element
 * containing a flat list of \c <item> elements in post-order DFS; each entry
 * holds its \c typeId, \c objectId, and optional \c <properties> and
 * \c <children> sub-elements.  Shared subtrees are deduplicated exactly as in
 * the JSON serializer – a child is only written once.
 *
 * Wire format:
 * \code
 * [DMFMT header — 23 bytes]
 * <?xml version="1.0" encoding="UTF-8"?>
 * <items>
 *   <item typeId="<uuid>" objectId="<uuid>" root="true">
 *     <properties>
 *       <property name="name" type="QString">Alice</property>
 *       <property name="tags" type="QStringList">
 *         <entry>fighter</entry>
 *       </property>
 *     </properties>
 *     <children>
 *       <child><childObjectId></child>
 *     </children>
 *   </item>
 * </items>
 * \endcode
 *
 * Child wiring during \c read() follows the same ItemResolver + processEvents()
 * mechanism as the JSON serializer.
 */
class XmlSerializer : public Serializer
{
  Q_OBJECT

public:
  explicit XmlSerializer();
  ~XmlSerializer() override;

  XmlSerializer(XmlSerializer const&) = delete;
  auto operator=(XmlSerializer const&) -> XmlSerializer& = delete;
  XmlSerializer(XmlSerializer&&) = delete;
  auto operator=(XmlSerializer&&) -> XmlSerializer& = delete;

  // ── Identity ──────────────────────────────────────────────────────────────

  static auto classId() -> boost::uuids::uuid;
  auto id() const -> boost::uuids::uuid override;

  // ── Capabilities ──────────────────────────────────────────────────────────

  auto filters() const -> QStringList override;
  auto provides(QString const& filter) const -> bool override;

  // ── IO ────────────────────────────────────────────────────────────────────

  auto read(FileHeader const& header, uint16_t version, std::istream& stream,
            infrastructure::ObjectFactory& factory, infrastructure::ObjectRegistry& registry) const
      -> std::vector<std::shared_ptr<infrastructure::TreeItem>> override;

  auto write(std::ostream& stream,
             std::vector<std::shared_ptr<infrastructure::TreeItem>> const& objects) const
      -> void override;

private:
  struct Impl;
  std::unique_ptr<Impl> _p;
};

} // namespace gurps_system
