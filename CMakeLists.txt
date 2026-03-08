cmake_minimum_required(VERSION 3.15...4.5)

list(APPEND CMAKE_PROJECT_TOP_LEVEL_INCLUDES ${CMAKE_CURRENT_LIST_DIR}/cmake/cmake-conan/conan_provider.cmake)
set(CONAN_BUILD_PROFILE "default;auto-cmake")
set(CONAN_INSTALL_ARGS "--build=missing")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

project(dungeonMaster VERSION 1.0 LANGUAGES CXX)

include(CTest)
include(utilities)
include(dependencies)

add_subdirectory(src)
if (BUILD_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()
