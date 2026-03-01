add_library(froute INTERFACE)

target_include_directories(froute INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/syoch-robotics/froute>
)
install(TARGETS froute
  EXPORT sroboTargets
  INCLUDES DESTINATION include/syoch-robotics/froute
)
install(DIRECTORY include/
  DESTINATION include/syoch-robotics/froute
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
