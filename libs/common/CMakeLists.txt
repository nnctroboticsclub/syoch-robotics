add_library(syoch-robotics-common STATIC
  src/node/node.cpp
  src/platform/panic.cpp
  src/types/angle_joystick_2d.cpp
  src/types/joystick_2d.cpp
  src/types/pid_gains.cpp
  src/network/can_base.cpp
)
add_library(syoch-robotics::common ALIAS syoch-robotics-common)

target_include_directories(syoch-robotics-common PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/syoch-robotics/common>
)

set_property(TARGET syoch-robotics-common PROPERTY CXX_STANDARD 20)

install(TARGETS syoch-robotics-common
  EXPORT sroboTargets
  INCLUDES DESTINATION include/syoch-robotics/common
)

install(DIRECTORY include/
  DESTINATION include/syoch-robotics/common
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
