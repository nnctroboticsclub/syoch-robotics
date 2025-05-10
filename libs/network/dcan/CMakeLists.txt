add_library(dcan STATIC
  source/dcan.cpp
  source/can_module.cpp
)
srobo1_target(dcan network)

target_link_libraries(dcan PUBLIC
  timer common
)