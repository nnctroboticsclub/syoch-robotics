if(NOT "IkakoRobomas" IN_LIST SYOCH_ROBOTICS_BYPASS_DEPS)
  find_package(ikarashiCAN_mk2 REQUIRED)
  find_package(IkakoRobomas REQUIRED)
  find_package(MotorController REQUIRED)
endif()

add_library(ikako-robomas INTERFACE)
srobo1_target(ikako-robomas drivers)

target_link_libraries(ikako-robomas INTERFACE
  ikarashiCAN_mk2
  IkakoRobomas
  MotorController

  common
  ${SYOCH_ROBOTICS_DEPENDENCIES}
)
