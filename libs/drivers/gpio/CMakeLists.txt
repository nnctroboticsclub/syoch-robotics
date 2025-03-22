add_library(syoch-robotics-gpio INTERFACE)

target_include_directories(syoch-robotics-gpio INTERFACE
  "./include"
)

target_link_libraries(syoch-robotics-gpio INTERFACE ${SYOCH_ROBOTICS_DEPENDENCIES})

if ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "Mbed")
  set(SYOCH_ROBOTICS_DRIVER_GPIO ON)
elseif ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "ESP")
  set(SYOCH_ROBOTICS_DRIVER_GPIO ON)
elseif ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "STM32 HAL")
  set(SYOCH_ROBOTICS_DRIVER_GPIO OFF)
elseif ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "Emulation")
  set(SYOCH_ROBOTICS_DRIVER_GPIO OFF)
else()
  message(FATAL_ERROR "Unknown SYOCH_ROBOTICS_TARGET: ${SYOCH_ROBOTICS_TARGET}")
endif()