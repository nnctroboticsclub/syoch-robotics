add_library(context-detector INTERFACE)
srobo1_target(context-detector binary)

if ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "Mbed")
  target_link_libraries(context-detector INTERFACE
    StaticMbedOS
  )
  target_compile_definitions(context-detector INTERFACE
    SR_BCD_CORTEX_M4
    SR_BCD_PL_TARGET_${MBED_TARGET}
  )
elseif ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "STM32 HAL")
  target_compile_definitions(context-detector INTERFACE
  SR_BCD_CORTEX_M4
  )
endif()
