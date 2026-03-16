my_add_test(SystemDatamodelTests
  AUTOMOC
  DEPENDS
    DM::System::Datamodel
    Qt6::Core
  INCLUDES
    PRIVATE ${CMAKE_SOURCE_DIR}/src/system/datamodel/src/
  SOURCE
    baseObjectTest.cpp
    formulaTest.cpp
    attributeTest.cpp
    derivationFormulaTest.cpp
    registrationTest.cpp
    skillTest.cpp
    skillDefaultTest.cpp
    prerequisiteTest.cpp
)
