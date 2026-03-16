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
    include/cpTable.hpp
    include/registration.hpp
    include/attributeContainer.hpp
    src/formula.hpp
    src/linearFormula.hpp
    src/lookupFormula.hpp
    src/parentRef.hpp
    src/derivationFormula.hpp
    src/scaledSumDerivationFormula.hpp
    src/quadraticDerivationFormula.hpp
    src/lookupDerivationFormula.hpp
  SOURCE
    src/baseObject.cpp
    src/formula.cpp
    src/linearFormula.cpp
    src/lookupFormula.cpp
    src/attribute.cpp
    src/parentRef.cpp
    src/derivationFormula.cpp
    src/scaledSumDerivationFormula.cpp
    src/quadraticDerivationFormula.cpp
    src/lookupDerivationFormula.cpp
    src/registration.cpp
    src/attributeContainer.cpp
  ALIAS DM::System::Datamodel
)
