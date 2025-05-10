add_library(common STATIC
  source/node/node.cpp
  source/platform/panic.cpp
  source/types/angle_joystick_2d.cpp
  source/types/joystick_2d.cpp
  source/types/pid_gains.cpp
  source/network/can_base.cpp
)

set_property(TARGET common PROPERTY CXX_STANDARD 20)

srobo1_target(common common)
