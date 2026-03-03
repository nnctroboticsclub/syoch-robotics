add_library(bd621x INTERFACE)
add_library(syoch-robotics::bd621x ALIAS bd621x)
target_include_directories(bd621x INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/syoch-robotics/bd621x>
)
target_link_libraries(bd621x INTERFACE
  Nano::NanoHW
  common
)

install(TARGETS bd621x
  EXPORT sroboTargets
  INCLUDES DESTINATION include/syoch-robotics/bd621x
)

install(DIRECTORY include/
  DESTINATION include/syoch-robotics/bd621x
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
