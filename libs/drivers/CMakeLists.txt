add_subdirectory(gpio)
add_subdirectory(bldc)
add_subdirectory(bd621x)

if ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "Mbed")
  add_subdirectory(ikako_robomas)
  add_subdirectory(ikako_mdc)
endif()