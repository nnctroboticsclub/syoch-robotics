add_library(context-detector INTERFACE)
add_library(syoch-robotics::context-detector ALIAS context-detector)

target_include_directories(context-detector INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/syoch-robotics/context-detector>
)

target_link_libraries(context-detector INTERFACE CMSIS5::CoreM)
target_compile_definitions(context-detector INTERFACE
  SR_BCD_CORTEX_M4
)

install(TARGETS context-detector
  EXPORT sroboTargets
  INCLUDES DESTINATION include/syoch-robotics/context-detector
)

install(DIRECTORY include/
  DESTINATION include/syoch-robotics/context-detector
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)