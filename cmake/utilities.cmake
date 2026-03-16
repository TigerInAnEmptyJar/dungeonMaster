set(CMAKE_INCLUDE_CURRENT_DIR ON)  # -> adds CMAKE_CURRENT_SOURCE_DIR and CMAKE_CURRENT_BINARY_DIR to the include path for each directory.  Useful mainly for out-of-source builds, where files generated into the build tree are included by files located in the source tree.

function(my_add_library _NAME)
  set(flags INTERFACE STATIC AUTORCC AUTOMOC AUTOUIC)
  set(single ALIAS )
  set(multi HEADER SOURCE RESOURCES FORMS DEPENDS INCLUDES DEFINES QMLS )
  cmake_parse_arguments(PARSE_ARGV 1 A "${flags}" "${single}" "${multi}")
  
  # Handle Qt resource (.qrc) files
  set(_qrc_resources "")
  if (A_RESOURCES)
    foreach(_qrc_file ${A_RESOURCES})
      qt6_add_resources(_qrc_output ${_qrc_file})
      list(APPEND _qrc_resources ${_qrc_output})
    endforeach()
  endif()
  
  # Handle QML files by creating a Qt resource
  set(_qml_resources "")
  if (A_QMLS)
    qt_add_resources(_qml_resources "${_NAME}_qml"
      PREFIX "/qml/${_NAME}"
      FILES ${A_QMLS}
    )
  endif()
  
  if(${A_INTERFACE})
    message(STATUS "Adding interface library ${_NAME} (alias: ${A_ALIAS})")
    add_library(${_NAME} INTERFACE ${A_HEADER})
  else()
    if(A_STATIC)
      message(STATUS "Adding static library ${_NAME} (alias: ${A_ALIAS})")
      add_library(${_NAME} STATIC ${A_HEADER} ${A_SOURCE} ${A_RESOURCES} ${A_FORMS} ${_qrc_resources} ${_qml_resources})
    else()
      message(STATUS "Adding shared library ${_NAME} (alias: ${A_ALIAS})")
      add_library(${_NAME} ${A_HEADER} ${A_SOURCE} ${A_RESOURCES} ${A_FORMS} ${_qrc_resources} ${_qml_resources})
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
    if (A_HEADER)
      qt6_wrap_cpp(_moc_sources ${A_HEADER} TARGET ${_NAME})
      target_sources(${_NAME} PRIVATE ${_moc_sources})
    endif()
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
  set(multi HEADER SOURCE RESOURCES FORMS DEPENDS INCLUDES DEFINES QMLS)
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
    if (A_HEADER)
      qt6_wrap_cpp(_moc_sources ${A_HEADER} TARGET ${_NAME})
      target_sources(${_NAME} PRIVATE ${_moc_sources})
    endif()
  endif()
  if (A_AUTOUIC)
    set_target_properties(${_NAME} PROPERTIES AUTOUIC ON)
  endif()

  set_target_properties(${_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/)

  gtest_add_tests(TARGET ${_NAME}
                  SOURCES ${A_SOURCE}
  )
endfunction()

function(my_add_qml_test _NAME)
  set(flags AUTOMOC)
  set(single SOURCE)
  set(multi QML_FILES DEPENDS INCLUDES DEFINES)
  cmake_parse_arguments(PARSE_ARGV 1 A "${flags}" "${single}" "${multi}")
  
  message(STATUS "Adding QML test ${_NAME}")
  
  # Create the test executable with Qt Quick Test runner
  add_executable(${_NAME} ${A_SOURCE})
  
  # Copy QML test files to build directory for Qt Quick Test discovery
  foreach(_qml_file ${A_QML_FILES})
    configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/${_qml_file}
      ${CMAKE_CURRENT_BINARY_DIR}/${_qml_file}
      COPYONLY
    )
  endforeach()
  
  # Set up dependencies - Qt Quick Test requires these modules
  set(dependencies Qt6::Core Qt6::Qml Qt6::Quick Qt6::QuickTest)
  if (A_DEPENDS)
    list(APPEND dependencies ${A_DEPENDS})
  endif()
  target_link_libraries(${_NAME} PRIVATE ${dependencies})
  
  if (A_INCLUDES)
    target_include_directories(${_NAME} ${A_INCLUDES})
  endif()
  
  if (A_DEFINES)
    target_compile_definitions(${_NAME} ${A_DEFINES})
  endif()
  
  # Qt Quick Test requires AUTOMOC
  set_target_properties(${_NAME} PROPERTIES
    AUTOMOC ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/
  )
  
  # Add the test to CTest
  add_test(NAME ${_NAME} COMMAND ${_NAME})

  # Set environment variable to use offscreen platform (no display needed)
  set_tests_properties(${_NAME} PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
  )

endfunction()

endif()