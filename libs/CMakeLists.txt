if ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "Mbed")
  find_package(StaticMbedOS REQUIRED)

  add_compile_definitions(__MBED__)
  set(SYOCH_ROBOTICS_DEPENDENCIES StaticMbedOS)
  set(NON_THREAD OFF CACHE BOOL "Thread Availability" FORCE)
elseif ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "ESP")
  add_compile_definitions(ESP_PLATFORM)

  set(SYOCH_ROBOTICS_DEPENDENCIES __idf_driver __idf_freertos __idf_esp_timer)
  list(APPEND COMPONENTS driver freertos esp_timer)
  set(NON_THREAD ON CACHE BOOL "Thread Availability" FORCE)
elseif ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "STM32 HAL")
  add_compile_definitions(__STM32__ __HAL__)
  set(SYOCH_ROBOTICS_DEPENDENCIES "")
  set(NON_THREAD ON CACHE BOOL "Thread Availability" FORCE)
elseif ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "Emulation")
  add_compile_definitions(__EMULATION__)
  set(SYOCH_ROBOTICS_DEPENDENCIES "")
  set(NON_THREAD OFF CACHE BOOL "Thread Availability" FORCE)
else()
  message(FATAL_ERROR "Unknown SYOCH_ROBOTICS_TARGET: ${SYOCH_ROBOTICS_TARGET}")
endif()



add_subdirectory(common)
add_subdirectory(binary)
add_subdirectory(logger)

if (NOT ${NON_THREAD})
  add_subdirectory(thread)
endif()

if ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "Mbed")
  add_subdirectory(mbed)
  add_subdirectory(random)
endif()

if (NOT "${SYOCH_ROBOTICS_TARGET}" STREQUAL "Emulation" AND NOT "${SYOCH_ROBOTICS_TARGET}" STREQUAL "STM32 HAL")
  add_subdirectory(drivers)
  add_subdirectory(network)
  add_subdirectory(timer)
endif()