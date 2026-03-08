my_add_test(SystemIoTests
  AUTOMOC
  DEPENDS
    DM::System::Io
    Qt6::Core
  SOURCE
    ioTest.cpp
)
