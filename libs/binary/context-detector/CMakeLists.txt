add_library(syoch-robotics-context-detector INTERFACE)
add_library(syoch-robotics::context-detector ALIAS syoch-robotics-context-detector)

target_include_directories(syoch-robotics-context-detector INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/syoch-robotics/context-detector>
)

target_link_libraries(syoch-robotics-context-detector INTERFACE CMSIS5::CoreM)
target_compile_definitions(syoch-robotics-context-detector INTERFACE
  SR_BCD_CORTEX_M4
)

install(TARGETS syoch-robotics-context-detector
  EXPORT sroboTargets
  INCLUDES DESTINATION include/syoch-robotics/syoch-robotics-context-detector
)

install(DIRECTORY include/
  DESTINATION include/syoch-robotics/context-detector
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)