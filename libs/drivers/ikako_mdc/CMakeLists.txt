add_library(syoch-robotics-ikako-mdc STATIC source/ikako_mdc.cpp)
add_library(syoch-robotics::ikako-mdc ALIAS syoch-robotics-ikako-mdc)

find_package(ikarashiCAN_mk2 REQUIRED)
find_package(ikakoMDC REQUIRED)

target_link_libraries(syoch-robotics-ikako-mdc PUBLIC
  dcan
  ikarashiCAN_mk2
  ikakoMDC
  syoch-robotics-common
)

target_include_directories(syoch-robotics-ikako-mdc PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/syoch-robotics/ikako-mdc>
)

install(TARGETS syoch-robotics-ikako-mdc
  EXPORT sroboTargets
  INCLUDES DESTINATION include/syoch-robotics/ikako-mdc
)

install(DIRECTORY include/
  DESTINATION include/syoch-robotics/ikako-mdc
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
