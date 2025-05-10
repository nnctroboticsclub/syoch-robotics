cmake_minimum_required(VERSION 3.25)
project(syoch-robotics VERSION 1.3.4 LANGUAGES C CXX ASM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

function(srobo1_target target component)
  get_target_property(type ${target} TYPE)
  if (${type} STREQUAL "INTERFACE_LIBRARY")
    set(include_type INTERFACE)
  else()
    set(include_type PUBLIC)
  endif()

  target_include_directories(${target} ${include_type}
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  )

  file(GLOB_RECURSE public_headers include/*.hpp)
  set_target_properties(${target} PROPERTIES PUBLIC_HEADER "${public_headers}")

  install(
    TARGETS ${target}
    EXPORT srobo1-targets
    ARCHIVE DESTINATION lib/srobo1/${component}
    PUBLIC_HEADER DESTINATION include/srobo1/robotics/${component}
    COMPONENT ${component}
  )
endfunction()

add_subdirectory(libs)

# if ${SYOCH_ROBOTICS_DEPENDENCIES} is not ""
message(STATUS "SYOCH_ROBOTICS_DEPENDENCIES is set to |${SYOCH_ROBOTICS_DEPENDENCIES}|")
if (NOT "${SYOCH_ROBOTICS_DEPENDENCIES}" STREQUAL "")
  install(
    TARGETS ${SYOCH_ROBOTICS_DEPENDENCIES} ${SYOCH_ROBOTICS_IGNORE}
    EXPORT dummy-targets
    ARCHIVE DESTINATION lib/srobo1/dummy
    PUBLIC_HEADER DESTINATION include/srobo1/robotics/dummy
  )
  install(
    EXPORT dummy-targets
    DESTINATION share/cmake/
    FILE Srobo1-dummy-config.cmake
  )
endif()

install(
  EXPORT srobo1-targets
  DESTINATION share/cmake/
  FILE Srobo1-config.cmake

  NAMESPACE SRobo1::
)

set(CPACK_VERBATIM_VARIABLES YES)
set(CPACK_PACKAGE_NAME "syoch-robotics")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/dist")
set(CPACK_PACKAGE_CONTACT "syoch64@gmail.com")
set(CPACK_GENERATOR "TGZ")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "syoch")

set(CPACK_SOURCE_PACKAGE_FILE_NAME "myapp-${CMAKE_PROJECT_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})

set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.4), libstdc++6 (>= 4.1.1)")
include(InstallRequiredSystemLibraries)
include(CPack)
