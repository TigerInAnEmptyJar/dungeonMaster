my_add_library(infrastructure
  STATIC AUTOMOC
  INCLUDES
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DEPENDS
    PUBLIC Qt6::Core
    PUBLIC Boost::headers
  HEADER
    include/treeItem.hpp
    include/objectRegistry.hpp
  SOURCE
    src/treeItem.cpp
    src/objectRegistry.cpp
  ALIAS DM::Infrastructure
)