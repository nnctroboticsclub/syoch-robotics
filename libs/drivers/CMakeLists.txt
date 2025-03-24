add_subdirectory(gpio)
add_subdirectory(bldc)

if(${SYOCH_ROBOTICS_DRIVER_GPIO})
  add_subdirectory(bd621x)
endif()

if ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "Mbed" OR "${SYOCH_ROBOTICS_IKAKO_ROBOMAS}")
  message(STATUS "Adding ikako_robomas")
  message(STATUS "  - SYOCH_ROBOTICS_TARGET: ${SYOCH_ROBOTICS_TARGET}")
  message(STATUS "  - SYOCH_ROBOTICS_IKAKO_ROBOMAS: ${SYOCH_ROBOTICS_IKAKO_ROBOMAS}")
  add_subdirectory(ikako_robomas)
endif()

if ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "Mbed")
  add_subdirectory(ikako_mdc)
endif()