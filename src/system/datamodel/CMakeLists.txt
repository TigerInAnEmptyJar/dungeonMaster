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
    include/profession.hpp
    include/professionContainer.hpp
    include/race.hpp
    include/raceContainer.hpp
    src/formula.hpp
    src/linearFormula.hpp
    src/lookupFormula.hpp
    src/parentRef.hpp
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
    src/scaledSumDerivationFormula.cpp
    src/quadraticDerivationFormula.cpp
    src/lookupDerivationFormula.cpp
    src/registration.cpp
    src/attributeContainer.cpp
    src/profession.cpp
    src/professionContainer.cpp
    src/race.cpp
    src/raceContainer.cpp
  ALIAS DM::System::Datamodel
)
