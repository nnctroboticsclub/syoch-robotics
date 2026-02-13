add_library(syoch-robotics-neopixel STATIC
  source/neopixel.cpp
)

target_link_libraries(syoch-robotics-neopixel PUBLIC
  syoch-robotics-common
  Nano::NanoHW
)

target_include_directories(syoch-robotics-neopixel PUBLIC
  "./include"
)