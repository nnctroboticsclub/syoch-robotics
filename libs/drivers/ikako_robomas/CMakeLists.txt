add_library(syoch-robotics-ikako-robomas INTERFACE)

if(NOT "IkakoRobomas" IN_LIST SYOCH_ROBOTICS_BYPASS_DEPS)
  find_package(ikarashiCAN_mk2 REQUIRED)
  find_package(IkakoRobomas REQUIRED)
  find_package(MotorController REQUIRED)
endif()

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