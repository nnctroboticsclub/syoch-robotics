add_library(dcan STATIC
  source/dcan.cpp
  source/can_module.cpp
)

target_link_libraries(dcan PUBLIC
  syoch-robotics-timer syoch-robotics-common
)

target_include_directories(dcan PUBLIC
  "./include"
)