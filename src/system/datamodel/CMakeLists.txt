my_add_library(systemDatamodel
  STATIC AUTOMOC
  INCLUDES
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include/
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/
  DEPENDS
    PUBLIC Qt6::Core
    PUBLIC DM::Infrastructure
  HEADER
    include/baseObject.hpp
    include/attribute.hpp
    include/registration.hpp
    src/formula.hpp
    src/linearFormula.hpp
    src/lookupFormula.hpp
  SOURCE
    src/baseObject.cpp
    src/formula.cpp
    src/linearFormula.cpp
    src/lookupFormula.cpp
    src/attribute.cpp
    src/registration.cpp
  ALIAS DM::System::Datamodel
)
