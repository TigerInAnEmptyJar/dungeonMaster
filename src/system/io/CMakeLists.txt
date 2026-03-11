my_add_library(systemIo
  STATIC AUTOMOC
  INCLUDES
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include/
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/
  DEPENDS
    PUBLIC Qt6::Core
    PUBLIC DM::Infrastructure
  HEADER
    include/serializer.hpp
    include/io.hpp
    include/jsonSerializer.hpp
  SOURCE
    src/serializer.cpp
    src/io.cpp
    src/jsonSerializer.cpp
  ALIAS DM::System::Io
)
