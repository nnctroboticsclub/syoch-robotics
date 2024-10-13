add_library(bd621x OBJECT
  source/BD621x.cpp
  source/BD621xFull.cpp
)

target_include_directories(bd621x PUBLIC
  "./include"
)

target_link_libraries(bd621x PRIVATE
  syoch-robotics-gpio
)

target_link_libraries(bd621x PUBLIC
  syoch-robotics-common
)