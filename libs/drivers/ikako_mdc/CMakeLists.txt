add_library(syoch-robotics-ikako-mdc STATIC
  source/ikako_mdc.cpp
)

target_link_libraries(syoch-robotics-ikako-mdc PUBLIC
  dcan
  ikarashiCAN_mk2
  ikakoMDC
  syoch-robotics-common
  ${SYOCH_ROBOTICS_DEPENDENCIES}
)

target_include_directories(syoch-robotics-ikako-mdc PUBLIC
  "./include"
)