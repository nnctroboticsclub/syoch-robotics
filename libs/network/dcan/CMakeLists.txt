add_library(dcan STATIC
  source/dcan.cpp
  source/can_base.cpp
  source/can_module.cpp
)

target_link_libraries(dcan PUBLIC
  syoch-robotics-timer
)

target_include_directories(dcan PUBLIC
  "./include"
)