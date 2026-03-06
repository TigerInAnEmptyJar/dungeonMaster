function(my_find_package _NAME)
  set(flags "")
  set(single "VERSION;LICENSE_PATH")
  set(multi "COMPONENTS")
  cmake_parse_arguments(A ${flags} ${single} ${multi} ${ARGN})
  message(STATUS "Adding dependency ${_NAME}")
  
  set(version "")
  if (A_VERSION)
    set(version "${A_VERSION}")
  endif()

  if (A_COMPONENTS)
    find_package(${_NAME} ${version} COMPONENTS ${A_COMPONENTS})
  else() 
    find_package(${_NAME} ${version})
  endif()
  string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE_UPPER)

  if (A_LICENSE_PATH)
    install(FILES ${${_NAME}_PACKAGE_FOLDER_${BUILD_TYPE_UPPER}}/${A_LICENSE_PATH} 
            DESTINATION ${CMAKE_INSTALL_PREFIX}/licenses/${_NAME}/License.txt)
  endif()
endfunction()

my_find_package(Qt6)