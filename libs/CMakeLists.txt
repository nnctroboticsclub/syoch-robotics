# SYOCH_ROBOTICS_DEPENDENCIES should be removed

add_subdirectory(common)
add_subdirectory(binary)
add_subdirectory(logger)
add_subdirectory(drivers)
add_subdirectory(random)

if ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "Mbed")
  add_subdirectory(mbed)
endif()

add_subdirectory(network)
add_subdirectory(timer)
