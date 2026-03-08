set(CMAKE_INCLUDE_CURRENT_DIR ON)  # -> adds CMAKE_CURRENT_SOURCE_DIR and CMAKE_CURRENT_BINARY_DIR to the include path for each directory.  Useful mainly for out-of-source builds, where files generated into the build tree are included by files located in the source tree.

function(my_add_library _NAME)
  set(flags INTERFACE STATIC AUTORCC AUTOMOC AUTOUIC)
  set(single ALIAS )
  set(multi HEADER SOURCE RESOURCES FORMS DEPENDS INCLUDES DEFINES QMLS )
  cmake_parse_arguments(PARSE_ARGV 1 A "${flags}" "${single}" "${multi}")
  if(${A_INTERFACE})
    message(STATUS "Adding interface library ${_NAME} (alias: ${A_ALIAS})")
    add_library(${_NAME} INTERFACE ${A_HEADER})
  else()
    if(A_STATIC)
      message(STATUS "Adding static library ${_NAME} (alias: ${A_ALIAS})")
      add_library(${_NAME} STATIC ${A_HEADER} ${A_SOURCE} ${A_RESOURCES} ${A_FORMS} ${A_QMLS})
    else()
      message(STATUS "Adding shared library ${_NAME} (alias: ${A_ALIAS})")
      add_library(${_NAME} ${A_HEADER} ${A_SOURCE} ${A_RESOURCES} ${A_FORMS} ${A_QMLS})
    endif()
  endif()

  if (A_DEPENDS)
    target_link_libraries(${_NAME} ${A_DEPENDS})
  endif()

  if (A_INCLUDES)
    target_include_directories(${_NAME} ${A_INCLUDES})
  endif()

  if (A_DEFINES)
    target_compile_definitions(${_NAME} ${A_DEFINES})
  endif()

  if (A_AUTORCC)
    set_target_properties(${_NAME} PROPERTIES AUTORCC ON)
  endif()
  if (A_AUTOMOC)
    set_target_properties(${_NAME} PROPERTIES AUTOMOC ON)
  endif()
  if (A_AUTOUIC)
    set_target_properties(${_NAME} PROPERTIES AUTOUIC ON)
  endif()

  if (A_ALIAS)
    add_library(${A_ALIAS} ALIAS ${_NAME})
  endif()
endfunction()

function(my_add_executable _NAME)
  set(flags  AUTORCC AUTOMOC AUTOUIC INSTALL)
  set(single )
  set(multi HEADER SOURCE RESOURCES FORMS DEPENDS INCLUDES DEFINES QMLS )
  cmake_parse_arguments(PARSE_ARGV 1 A "${flags}" "${single}" "${multi}")

  message(STATUS "Adding executable ${_NAME}")
  add_executable(${_NAME} ${A_HEADER} ${A_SOURCE} ${A_RESOURCES} ${A_FORMS} ${A_QMLS})

  if (A_DEPENDS)
    target_link_libraries(${_NAME} ${A_DEPENDS})
  endif()

  if (A_INCLUDES)
    target_include_directories(${_NAME} ${A_INCLUDES})
  endif()

  if (A_DEFINES)
    target_compile_definitions(${_NAME} ${A_DEFINES})
  endif()

  if (A_AUTORCC)
    set_target_properties(${_NAME} PROPERTIES AUTORCC ON)
  endif()
  if (A_AUTOMOC)
    set_target_properties(${_NAME} PROPERTIES AUTOMOC ON)
  endif()
  if (A_AUTOUIC)
    set_target_properties(${_NAME} PROPERTIES AUTOUIC ON)
  endif()
  if (A_INSTALL)
    install(TARGET ${NAME})
  endif()

  set_target_properties(${_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

endfunction()

if (BUILD_TESTING)
include(GoogleTest)

function(my_add_test _NAME)
  set(flags  AUTORCC AUTOMOC AUTOUIC)
  set(single )
  set(multi HEADER SOURCE RESOURCES FORMS DEPENDS INCLUDES DEFINES QMLS )
  cmake_parse_arguments(PARSE_ARGV 1 A "${flags}" "${single}" "${multi}")
  message(STATUS "Adding tests ${_NAME}")
  add_executable(${_NAME} ${A_HEADER} ${A_SOURCE} ${A_RESOURCES} ${A_FORMS} ${A_QMLS})

  set(dependencies GTest::Main GTest::gmock)
  if (A_DEPENDS)
    list(APPEND dependencies ${A_DEPENDS})
  endif()
  target_link_libraries(${_NAME} ${dependencies})

  if (A_INCLUDES)
    target_include_directories(${_NAME} ${A_INCLUDES})
  endif()

  if (A_DEFINES)
    target_compile_definitions(${_NAME} ${A_DEFINES})
  endif()

  if (A_AUTORCC)
    set_target_properties(${_NAME} PROPERTIES AUTORCC ON)
  endif()
  if (A_AUTOMOC)
    set_target_properties(${_NAME} PROPERTIES AUTOMOC ON)
  endif()
  if (A_AUTOUIC)
    set_target_properties(${_NAME} PROPERTIES AUTOUIC ON)
  endif()

  set_target_properties(${_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/)

  gtest_add_tests(TARGET ${_NAME}
                  SOURCES ${A_SOURCE}
  )
endfunction()
endif()