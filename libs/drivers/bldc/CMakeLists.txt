add_library(syoch-robotics-bldc STATIC
  source/BLDC.cpp
)

target_link_libraries(syoch-robotics-bldc PUBLIC
  ${SYOCH_ROBOTICS_DEPENDENCIES}
  syoch-robotics-common
  syoch-robotics-gpio
)

target_include_directories(syoch-robotics-bldc PUBLIC
  "./include"
)