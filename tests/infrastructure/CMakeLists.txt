my_add_test(InfrastructureTests
  AUTOMOC
  DEPENDS
    DM::Infrastructure
    Qt6::Core
  SOURCE
    treeItemTest.cpp
)
