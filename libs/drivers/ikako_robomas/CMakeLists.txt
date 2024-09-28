add_library(syoch-robotics-ikako-robomas INTERFACE)

target_link_libraries(syoch-robotics-ikako-robomas INTERFACE
  ikarashiCAN_mk2
  IkakoRobomas
  MotorController
  syoch-robotics-common
  ${SYOCH_ROBOTICS_DEPENDENCIES}
)

target_include_directories(syoch-robotics-ikako-robomas INTERFACE
  "./include"
)