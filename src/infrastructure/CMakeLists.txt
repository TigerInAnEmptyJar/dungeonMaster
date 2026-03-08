my_add_library(infrastructure
  STATIC AUTOMOC
  INCLUDES
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DEPENDS
    PUBLIC Qt6::Core
  HEADER
    include/treeItem.hpp
  SOURCE
    src/treeItem.cpp
  ALIAS DM::Infrastructure
)