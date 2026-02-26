add_library(syoch-robotics-ikako-robomas INTERFACE)
add_library(syoch-robotics::ikako-robomas ALIAS syoch-robotics-ikako-robomas)

find_package(ikarashiCAN_mk2 REQUIRED)
find_package(IkakoRobomas REQUIRED)
find_package(MotorController REQUIRED)

target_link_libraries(syoch-robotics-ikako-robomas INTERFACE
  ikarashiCAN_mk2
  IkakoRobomas
  MotorController
  syoch-robotics-common
)

target_include_directories(syoch-robotics-ikako-robomas INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/syoch-robotics/ikako-robomas>
)

install(TARGETS syoch-robotics-ikako-robomas
  EXPORT sroboTargets
  INCLUDES DESTINATION include/syoch-robotics/ikako-robomas
)

install(DIRECTORY include/
  DESTINATION include/syoch-robotics/ikako-robomas
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
