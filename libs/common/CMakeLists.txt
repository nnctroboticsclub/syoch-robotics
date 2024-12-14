add_library(syoch-robotics-common STATIC
  source/node/node.cpp
  source/platform/panic.cpp
  source/types/angle_joystick_2d.cpp
  source/types/joystick_2d.cpp
  source/types/pid_gains.cpp
)

target_include_directories(syoch-robotics-common PUBLIC
  "./include"
)

set_property(TARGET syoch-robotics-common PROPERTY CXX_STANDARD 20)
