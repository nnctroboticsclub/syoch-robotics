find_package(ikarashiCAN_mk2 REQUIRED)
find_package(ikakoMDC REQUIRED)

add_library(ikako-mdc STATIC source/ikako_mdc.cpp)
srobo1_target(ikako-mdc drivers)

target_link_libraries(ikako-mdc PUBLIC
  ikarashiCAN_mk2
  ikakoMDC
  dcan common
  ${SYOCH_ROBOTICS_DEPENDENCIES}
)
