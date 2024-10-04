add_library(syoch-robotics-simple-can STATIC)

target_link_libraries(syoch-robotics-simple-can PRIVATE
  StaticMbedOS
)

target_link_libraries(syoch-robotics-simple-can PUBLIC
  dcan
)

target_include_directories(syoch-robotics-simple-can PUBLIC
  "./include"
)

target_sources(syoch-robotics-simple-can PRIVATE
  source/simple_can.cpp
)