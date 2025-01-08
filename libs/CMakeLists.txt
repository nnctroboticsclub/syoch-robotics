if ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "Mbed")
  find_package(StaticMbedOS REQUIRED)

  add_compile_definitions(
    __MBED__
  )

  set(SYOCH_ROBOTICS_DEPENDENCIES
    "${SYOCH_ROBOTICS_DEPENDENCIES}"
    StaticMbedOS
  )
elseif ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "ESP")
  add_compile_definitions(
    ESP_PLATFORM
  )

  set(SYOCH_ROBOTICS_DEPENDENCIES
    "${SYOCH_ROBOTICS_DEPENDENCIES}"
    __idf_driver
    __idf_freertos
    __idf_esp_timer
  )
elseif ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "STM32 HAL")
  add_compile_definitions(
    __STM32__
    __HAL__
  )

  set(SYOCH_ROBOTICS_DEPENDENCIES
    "${SYOCH_ROBOTICS_DEPENDENCIES}"
  )
elseif ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "Emulation")
  add_compile_definitions(
    __EMULATION__
  )

  set(SYOCH_ROBOTICS_DEPENDENCIES
    "${SYOCH_ROBOTICS_DEPENDENCIES}"
  )
else()
  message(FATAL_ERROR "Unknown SYOCH_ROBOTICS_TARGET: ${SYOCH_ROBOTICS_TARGET}")
endif()



add_subdirectory(common)
add_subdirectory(binary)
add_subdirectory(logger)
add_subdirectory(thread)

if ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "Mbed")
  add_subdirectory(mbed)
endif()

if (NOT "${SYOCH_ROBOTICS_TARGET}" STREQUAL "Emulation")
  add_subdirectory(drivers)
  add_subdirectory(network)
  add_subdirectory(timer)
  add_subdirectory(random)
endif()