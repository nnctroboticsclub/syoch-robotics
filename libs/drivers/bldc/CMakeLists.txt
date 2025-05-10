add_library(bldc STATIC
  source/BLDC.cpp
)

target_link_libraries(bldc PUBLIC
  ${SYOCH_ROBOTICS_DEPENDENCIES}
  common gpio
)

srobo1_target(bldc drivers)