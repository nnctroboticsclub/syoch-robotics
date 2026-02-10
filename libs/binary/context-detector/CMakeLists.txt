add_library(syoch-robotics-context-detector INTERFACE)
target_include_directories(syoch-robotics-context-detector INTERFACE include)

target_link_libraries(syoch-robotics-context-detector INTERFACE CMSIS5::CoreM)
target_compile_definitions(syoch-robotics-context-detector INTERFACE
  SR_BCD_CORTEX_M4
)