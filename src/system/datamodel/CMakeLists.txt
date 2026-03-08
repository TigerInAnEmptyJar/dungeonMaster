my_add_library(systemDatamodel
  STATIC AUTOMOC
  INCLUDES
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DEPENDS
    PUBLIC Qt6::Core
    PUBLIC DM::Infrastructure
  HEADER
    include/baseObject.hpp
  SOURCE
    src/baseObject.cpp
  ALIAS DM::System::Datamodel
)
