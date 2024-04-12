idf_component_register(
  SRC_DIRS
    robotics
    robotics/sensor
    robotics/sensor/gyro
    robotics/registry
    robotics/types
    robotics/assembly
    robotics/node
    robotics/platform
    robotics/component
    robotics/component/swerve
    robotics/utils
    robotics/filter
    robotics/network
    robotics/controller
  INCLUDE_DIRS "."
  REQUIRES driver esp-idf-cxx
)
