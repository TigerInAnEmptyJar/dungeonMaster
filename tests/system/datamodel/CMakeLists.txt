my_add_test(SystemDatamodelTests
  AUTOMOC
  DEPENDS
    DM::System::Datamodel
    Qt6::Core
  SOURCE
    baseObjectTest.cpp
)
