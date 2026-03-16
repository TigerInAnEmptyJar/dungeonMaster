my_add_test(SystemIoTests
  AUTOMOC
  DEPENDS
    DM::System::Io
    DM::System::Datamodel
    Qt6::Core
  INCLUDES
    PRIVATE ${CMAKE_SOURCE_DIR}/src/system/datamodel/src/
  SOURCE
    ioTest.cpp
    jsonSerializerTest.cpp
    xmlSerializerTest.cpp
    registrationRoundTripTest.cpp
    gurps4AttributesTest.cpp
  HEADER
    testTypes.hpp
)
