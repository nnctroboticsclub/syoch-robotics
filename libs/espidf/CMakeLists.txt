idf_component_register(
  SRC_DIRS
    robotics
    robotics/sensor
    robotics/sensor/gyro
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
    idf-robotics
  INCLUDE_DIRS "."
  REQUIRES driver esp-idf-cxx
)
