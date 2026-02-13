# elseif ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "ESP")
#   set(SYOCH_ROBOTICS_DEPENDENCIES __idf_driver __idf_freertos __idf_esp_timer)
#   list(APPEND COMPONENTS driver freertos esp_timer)

# SYOCH_ROBOTICS_DEPENDENCIES should be removed



add_subdirectory(common)
add_subdirectory(binary)
add_subdirectory(logger)
add_subdirectory(random)
add_subdirectory(drivers)
add_subdirectory(thread)

if ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "Mbed")
  add_subdirectory(mbed)
endif()

add_subdirectory(network)
add_subdirectory(timer)
