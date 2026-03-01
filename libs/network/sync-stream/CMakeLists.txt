add_library(syoch-robotics-sync-stream INTERFACE)
add_library(syoch-robotics::sync-stream ALIAS syoch-robotics-sync-stream)

target_include_directories(syoch-robotics-sync-stream INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/syoch-robotics/sync>
)

install(TARGETS syoch-robotics-sync-stream
  EXPORT sroboTargets
  INCLUDES DESTINATION include/syoch-robotics/sync-stream
)
install(DIRECTORY include/
  DESTINATION include/syoch-robotics/sync-stream
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
