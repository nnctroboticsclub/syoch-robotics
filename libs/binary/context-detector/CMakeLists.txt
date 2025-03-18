add_library(syoch-robotics-context-detector INTERFACE)
target_include_directories(syoch-robotics-context-detector INTERFACE include)

if ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "Mbed")
  target_link_libraries(syoch-robotics-context-detector INTERFACE
    StaticMbedOS
  )
  target_compile_definitions(syoch-robotics-context-detector INTERFACE
    SR_BCD_CORTEX_M4
    SR_BCD_PL_TARGET_${MBED_TARGET}
  )
elseif ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "STM32 HAL")
  target_compile_definitions(syoch-robotics-context-detector INTERFACE
  SR_BCD_CORTEX_M4
  )
endif()