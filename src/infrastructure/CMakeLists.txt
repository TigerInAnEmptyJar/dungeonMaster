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
    include/objectFactory.hpp
    include/itemResolver.hpp
  SOURCE
    src/treeItem.cpp
    src/objectRegistry.cpp
    src/objectFactory.cpp
    src/itemResolver.cpp
  ALIAS DM::Infrastructure
)