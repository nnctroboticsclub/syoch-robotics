add_library(syoch-robotics-gpio INTERFACE)

target_include_directories(syoch-robotics-gpio INTERFACE
  "./include"
)

target_link_libraries(syoch-robotics-gpio INTERFACE
  ${SYOCH_ROBOTICS_DEPENDENCIES}
)