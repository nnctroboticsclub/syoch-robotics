add_library(syoch-robotics-bldc INTERFACE)
add_library(syoch-robotics::bldc ALIAS syoch-robotics-bldc)

target_link_libraries(syoch-robotics-bldc INTERFACE
  Nano::NanoHW
  syoch-robotics-common
)

target_include_directories(syoch-robotics-bldc INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/syoch-robotics/bldc>
)

install(TARGETS syoch-robotics-bldc
  EXPORT sroboTargets
  INCLUDES DESTINATION include/syoch-robotics/bldc
)

install(DIRECTORY include/
  DESTINATION include/syoch-robotics/bldc
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
