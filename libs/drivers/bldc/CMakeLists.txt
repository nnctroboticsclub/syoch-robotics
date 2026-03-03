add_library(bldc INTERFACE)
add_library(syoch-robotics::bldc ALIAS bldc)

target_link_libraries(bldc INTERFACE
  Nano::NanoHW
  common
)

target_include_directories(bldc INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/syoch-robotics/bldc>
)

install(TARGETS bldc
  EXPORT sroboTargets
  INCLUDES DESTINATION include/syoch-robotics/bldc
)

install(DIRECTORY include/
  DESTINATION include/syoch-robotics/bldc
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
