#include <attributeListModel.hpp>

#include <attribute.hpp>
#include <attributeContainer.hpp>
#include <baseObject.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <QCoreApplication>
#include <QModelIndex>

#include <memory>

/**
 * \brief Unit tests for AttributeListModel using GMock for signal verification.
 *
 * These tests use GMock's MockModelReceiver instead of QSignalSpy for verifying
 * Qt signals. This approach provides several advantages:
 *
 * 1. **Better parameter verification**: GMock matchers allow precise checking of
 *    signal parameters (e.g., testing::Contains, testing::AllOf).
 *
 * 2. **Clearer expectations**: EXPECT_CALL makes it explicit what signals are
 *    expected with what parameters.
 *
 * 3. **Automatic verification**: GMock automatically checks expectations in the
 *    test fixture destructor.
 *
 * 4. **Advanced matching**: Can use matchers like testing::_, testing::Not,
 *    testing::Contains to verify complex signal parameter combinations.
 *
 * Example: Instead of:
 *   QSignalSpy spy(model, &Model::dataChanged);
 *   // ... trigger signal ...
 *   EXPECT_EQ(spy.count(), 1);
 *
 * We use:
 *   EXPECT_CALL(*mockReceiver, onDataChanged(testing::_, testing::_,
 *               testing::Contains(NameRole))).Times(1);
 *   // ... trigger signal ...
 */

using testing::_;
using testing::Contains;
using testing::StrictMock;
namespace {

class MockModelReceiver
{
public:
  MOCK_METHOD(void, onRowsInserted, (QModelIndex const& parent, int first, int last));
  MOCK_METHOD(void, onRowsRemoved, (QModelIndex const& parent, int first, int last));
  MOCK_METHOD(void, onDataChanged,
              (QModelIndex const& topLeft, QModelIndex const& bottomRight,
               QList<int> const& roles));

  MockModelReceiver(QAbstractItemModel& model)
  {
    QObject::connect(&model, &QAbstractItemModel::rowsInserted,
                     [this](auto a, auto b, auto c) { onRowsInserted(a, b, c); });
    QObject::connect(&model, &QAbstractItemModel::rowsRemoved,
                     [this](auto a, auto b, auto c) { onRowsRemoved(a, b, c); });
    QObject::connect(&model, &QAbstractItemModel::dataChanged,
                     [this](auto a, auto b, auto c) { onDataChanged(a, b, c); });
  }
};

// ── Test fixture ──────────────────────────────────────────────────────────────

class AttributeListModelTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    container = std::make_shared<gurps_system::AttributeContainer>();
    model = std::make_unique<gurps_system::gui::AttributeListModel>();
    mockReceiver = std::make_unique<StrictMock<MockModelReceiver>>(*model);
  }

  void TearDown() override
  {
    mockReceiver.reset();
    model.reset();
    container.reset();
  }

  std::shared_ptr<gurps_system::AttributeContainer> container;
  std::unique_ptr<gurps_system::gui::AttributeListModel> model;
  std::unique_ptr<MockModelReceiver> mockReceiver;
};

// ── Tests ─────────────────────────────────────────────────────────────────────

TEST_F(AttributeListModelTest, InitiallyEmpty) { EXPECT_EQ(model->rowCount(), 0); }

TEST_F(AttributeListModelTest, SetContainer)
{
  model->setContainer(container.get());
  EXPECT_EQ(model->container(), container.get());
  EXPECT_EQ(model->rowCount(), 0);
}

TEST_F(AttributeListModelTest, ContainerWithAttributes)
{
  auto attr1 = std::make_shared<gurps_system::Attribute>();
  attr1->setName("Strength");
  attr1->setDescription("Physical power");
  container->insertChild(0, attr1);

  auto attr2 = std::make_shared<gurps_system::Attribute>();
  attr2->setName("Dexterity");
  attr2->setDescription("Agility");
  container->insertChild(1, attr2);

  model->setContainer(container.get());

  EXPECT_EQ(model->rowCount(), 2);
}

TEST_F(AttributeListModelTest, DataAccess)
{
  auto attr = std::make_shared<gurps_system::Attribute>();
  attr->setName("Intelligence");
  attr->setDescription("Mental ability");
  container->insertChild(0, attr);

  model->setContainer(container.get());

  auto index = model->index(0, 0);
  EXPECT_TRUE(index.isValid());

  auto nameData = model->data(index, gurps_system::gui::AttributeListModel::NameRole);
  EXPECT_EQ(nameData.toString().toStdString(), "Intelligence");

  auto descData = model->data(index, gurps_system::gui::AttributeListModel::DescriptionRole);
  EXPECT_EQ(descData.toString().toStdString(), "Mental ability");
}

TEST_F(AttributeListModelTest, AttributeObjectRole)
{
  auto attr = std::make_shared<gurps_system::Attribute>();
  attr->setName("Health");
  container->insertChild(0, attr);

  model->setContainer(container.get());

  auto index = model->index(0, 0);
  auto attrData = model->data(index, gurps_system::gui::AttributeListModel::AttributeObjectRole);

  EXPECT_TRUE(attrData.isValid());
  auto* retrievedAttr = attrData.value<gurps_system::Attribute*>();
  EXPECT_EQ(retrievedAttr, attr.get());
  EXPECT_EQ(retrievedAttr->name().toStdString(), "Health");
}

TEST_F(AttributeListModelTest, RoleNames)
{
  auto roles = model->roleNames();

  EXPECT_TRUE(roles.contains(gurps_system::gui::AttributeListModel::NameRole));
  EXPECT_TRUE(roles.contains(gurps_system::gui::AttributeListModel::DescriptionRole));
  EXPECT_TRUE(roles.contains(gurps_system::gui::AttributeListModel::AttributeObjectRole));

  EXPECT_EQ(roles[gurps_system::gui::AttributeListModel::NameRole], "name");
  EXPECT_EQ(roles[gurps_system::gui::AttributeListModel::DescriptionRole], "description");
  EXPECT_EQ(roles[gurps_system::gui::AttributeListModel::AttributeObjectRole], "attributeObject");
}

TEST_F(AttributeListModelTest, DynamicInsertion)
{
  model->setContainer(container.get());
  EXPECT_EQ(model->rowCount(), 0);

  // Expect rowsInserted signal with parent=invalid, first=0, last=0
  EXPECT_CALL(*mockReceiver, onRowsInserted(testing::_, 0, 0)).Times(1);

  auto attr = std::make_shared<gurps_system::Attribute>();
  attr->setName("Will");
  container->insertChild(0, attr);

  EXPECT_EQ(model->rowCount(), 1);
}

TEST_F(AttributeListModelTest, DynamicRemoval)
{
  auto attr = std::make_shared<gurps_system::Attribute>();
  attr->setName("Perception");
  container->insertChild(0, attr);

  model->setContainer(container.get());
  EXPECT_EQ(model->rowCount(), 1);

  // Expect rowsRemoved signal with parent=invalid, first=0, last=0
  EXPECT_CALL(*mockReceiver, onRowsRemoved(testing::_, 0, 0)).Times(1);

  container->removeChild(attr);

  EXPECT_EQ(model->rowCount(), 0);
}

TEST_F(AttributeListModelTest, PropertyChangeNotification)
{
  auto attr = std::make_shared<gurps_system::Attribute>();
  attr->setName("Original Name");
  container->insertChild(0, attr);

  model->setContainer(container.get());

  // Expect dataChanged signal with roles containing NameRole
  EXPECT_CALL(*mockReceiver,
              onDataChanged(testing::_, testing::_,
                            testing::Contains(gurps_system::gui::AttributeListModel::NameRole)))
      .Times(1);

  attr->setName("Updated Name");

  auto index = model->index(0, 0);
  auto nameData = model->data(index, gurps_system::gui::AttributeListModel::NameRole);
  EXPECT_EQ(nameData.toString().toStdString(), "Updated Name");
}

TEST_F(AttributeListModelTest, InvalidIndex)
{
  model->setContainer(container.get());

  auto invalidIndex = model->index(-1, 0);
  EXPECT_FALSE(invalidIndex.isValid());

  auto data = model->data(invalidIndex, gurps_system::gui::AttributeListModel::NameRole);
  EXPECT_FALSE(data.isValid());
}

TEST_F(AttributeListModelTest, NullContainer)
{
  model->setContainer(nullptr);
  EXPECT_EQ(model->container(), nullptr);
  EXPECT_EQ(model->rowCount(), 0);
}

TEST_F(AttributeListModelTest, MultipleInsertionsWithCorrectIndices)
{
  model->setContainer(container.get());

  // Expect first insertion at index 0
  EXPECT_CALL(*mockReceiver, onRowsInserted(testing::_, 0, 0)).Times(1);

  auto attr1 = std::make_shared<gurps_system::Attribute>();
  attr1->setName("First");
  container->insertChild(0, attr1);

  testing::Mock::VerifyAndClearExpectations(mockReceiver.get());

  // Expect second insertion at index 1
  EXPECT_CALL(*mockReceiver, onRowsInserted(testing::_, 1, 1)).Times(1);

  auto attr2 = std::make_shared<gurps_system::Attribute>();
  attr2->setName("Second");
  container->insertChild(1, attr2);

  EXPECT_EQ(model->rowCount(), 2);
}

TEST_F(AttributeListModelTest, DescriptionChangeEmitsCorrectRole)
{
  auto attr = std::make_shared<gurps_system::Attribute>();
  attr->setName("Test Attribute");
  attr->setDescription("Original Description");
  container->insertChild(0, attr);

  model->setContainer(container.get());

  // Expect dataChanged signal with roles containing DescriptionRole (not NameRole)
  EXPECT_CALL(
      *mockReceiver,
      onDataChanged(
          testing::_, testing::_,
          testing::AllOf(
              testing::Contains(gurps_system::gui::AttributeListModel::DescriptionRole),
              testing::Not(testing::Contains(gurps_system::gui::AttributeListModel::NameRole)))))
      .Times(1);

  attr->setDescription("New Description");

  auto index = model->index(0, 0);
  auto descData = model->data(index, gurps_system::gui::AttributeListModel::DescriptionRole);
  EXPECT_EQ(descData.toString().toStdString(), "New Description");
}

} // namespace
